/**
 * @file posix_main.cpp
 * @brief Multi-GPU POSIX checkpoint benchmark without MPI.
 *
 * One CPU thread per GPU performs D2H into host memory. The resulting buffers
 * are then written using individual files + merge, pwrite offsets, or a mutex-
 * serialized shared FILE*.
 */

#include "benchmark/bench_params.h"
#include "io/posix_file_write.h"
#include "output/csv.h"
#include "output/report.h"
#include "transfer/d2h_transfer.h"
#include "validation/runtime_checks.h"

#include <algorithm>
#include <cstdio>
#include <thread>
#include <vector>

namespace {

constexpr const char* CSV_FILE = "posix_bench.csv";

const char* const POSIX_DESCRIPTIONS[POSIX_NSTRATS] = {
    "one file per GPU followed by a streamed merge",
    "concurrent pwrite calls to fixed non-overlapping offsets",
    "shared FILE* protected by a mutex"
};

/** @brief Run concurrent D2H copies on every visible GPU. */
double setup_d2h_buffers(int device_count, std::size_t size, AllocType alloc,
                         std::vector<HostBuffer>* buffers)
{
    buffers->assign(static_cast<std::size_t>(device_count), HostBuffer{});
    std::vector<double> times(static_cast<std::size_t>(device_count), 0.0);
    std::vector<std::thread> workers;
    workers.reserve(static_cast<std::size_t>(device_count));

    for (int d = 0; d < device_count; ++d) {
        workers.emplace_back([&, d]() {
            HIP_CHECK(hipSetDevice(d));
            times[static_cast<std::size_t>(d)] = run_d2h_transfer(
                d, size, alloc, &(*buffers)[static_cast<std::size_t>(d)]);
        });
    }
    for (auto& worker : workers) worker.join();
    return *std::max_element(times.begin(), times.end());
}

} // namespace

int main()
{
    std::setbuf(stdout, nullptr);
    std::setbuf(stderr, nullptr);

    int device_count = 0;
    HIP_CHECK(hipGetDeviceCount(&device_count));
    if (device_count < 2) {
        std::fprintf(stderr, "This benchmark requires at least two GPUs/GCDs\n");
        return 1;
    }

    std::printf("POSIX Multi-GPU Write Benchmark — no MPI\n");
    std::printf("=========================================\n");
    print_device_banner(device_count);
    print_run_config("GPUs", device_count, NRUNS);
    print_strategy_legend(POSIX_STRAT_INFO, POSIX_DESCRIPTIONS, POSIX_NSTRATS);
    print_allocation_legend();
    csv_write_header(CSV_FILE, benchmark_csv_header(POSIX_STRAT_INFO, POSIX_NSTRATS));

    for (std::size_t s = 0; s < N_BENCHMARK_SIZES; ++s) {
        const std::size_t size = BENCHMARK_SIZES[s];
        const std::string size_text = format_size(size);

        for (std::size_t a = 0; a < N_BENCHMARK_ALLOCS; ++a) {
            const AllocType alloc = BENCHMARK_ALLOCS[a];
            std::vector<HostBuffer> buffers;
            const double wall_d2h = setup_d2h_buffers(device_count, size, alloc, &buffers);
            const double d2h_bw = (static_cast<double>(size) / 1e9) /
                                  (wall_d2h / 1e3);

            std::vector<float*> pointers(static_cast<std::size_t>(device_count));
            for (int d = 0; d < device_count; ++d) {
                pointers[static_cast<std::size_t>(d)] =
                    buffers[static_cast<std::size_t>(d)].data;
            }

            StratResult results[POSIX_NSTRATS]{};
            for (int st = 0; st < POSIX_NSTRATS; ++st) {
                results[st] = benchmark_posix_strategy(
                    pointers.data(), device_count, size, alloc,
                    static_cast<PosixWriteStrategy>(st));
            }

            std::printf("%-8s  %-8s  D2H: %7.2f ms  %6.2f GB/s\n",
                        size_text.c_str(), ALLOC_INFO[alloc].name,
                        wall_d2h, d2h_bw);
            for (int st = 0; st < POSIX_NSTRATS; ++st) {
                print_strat_result(POSIX_STRAT_INFO[st].name, results[st]);
            }
            std::printf("\n");
            csv_append_row(CSV_FILE,
                benchmark_csv_row(size_text, alloc, wall_d2h, d2h_bw,
                                  results, POSIX_STRAT_INFO, POSIX_NSTRATS));

            for (HostBuffer& buffer : buffers) release_host_buffer(&buffer);
        }
    }

    std::printf("CSV written to: %s\n", CSV_FILE);
    return 0;
}
