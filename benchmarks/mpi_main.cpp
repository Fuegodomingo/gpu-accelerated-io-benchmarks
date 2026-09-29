/**
 * @file mpi_main.cpp
 * @brief MPI-IO GPU-to-Lustre checkpoint benchmark.
 *
 * Each MPI rank owns one GCD, performs a D2H copy, then writes a non-overlapping
 * region of one shared file using Independent, Collective, and Direct-I/O MPI.
 */

#include "benchmark/bench_params.h"
#include "benchmark/stats.h"
#include "benchmark/mpi_timer.h"
#include "io/mpi_file_write.h"
#include "output/csv.h"
#include "output/report.h"
#include "transfer/d2h_transfer.h"
#include "validation/runtime_checks.h"
#include "validation/verify.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <mpi.h>

namespace {

constexpr const char* CSV_FILE = "mpi_bench.csv";

const char* const MPI_DESCRIPTIONS[MPI_NSTRATS] = {
    "MPI_File_write_at, page cache enabled",
    "MPI_File_write_at_all, ROMIO two-phase I/O",
    "MPI_File_write_at with direct_io=true"
};

/** @brief Print the effective Direct-I/O hint and Lustre file layout. */
void report_file_configuration(MPI_File fh, const char* path, MPIWriteStrategy strat, int rank)
{
    if (rank != 0) return;

    if (strat == MPI_STRAT_DIRECT) {
        MPI_Info info{};
        MPI_CHECK(MPI_File_get_info(fh, &info));
        char value[MPI_MAX_INFO_VAL]{};
        int found = 0;
        MPI_CHECK(MPI_Info_get(info, "direct_io", MPI_MAX_INFO_VAL - 1, value, &found));
        std::printf("  direct_io hint reported by ROMIO: %s\n", found ? value : "<not reported>");
        MPI_CHECK(MPI_Info_free(&info));
    }

    char command[512];
    std::snprintf(command, sizeof(command), "lfs getstripe '%s' 2>/dev/null || true", path);
    std::system(command);
}

/** @brief Benchmark one MPI-IO strategy, including durable MPI_File_sync in timing. */
StratResult benchmark_strategy(const char* path, int rank, int nranks,
                               const float* h_buf, std::size_t size,
                               MPIWriteStrategy strat)
{
    MPI_File fh = open_shared_file(path, nranks, static_cast<MPI_Offset>(size), strat);
    report_file_configuration(fh, path, strat, rank);

    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    write_data(fh, h_buf, size, rank, strat);
    MPI_CHECK(MPI_File_sync(fh));
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));

    double runs[NRUNS]{};
    for (int r = 0; r < NRUNS; ++r) {
        const double t0 = mpi_region_begin(MPI_COMM_WORLD);
        write_data(fh, h_buf, size, rank, strat);
        MPI_CHECK(MPI_File_sync(fh));
        runs[r] = mpi_region_end_ms(t0, MPI_COMM_WORLD);
    }

    MPI_CHECK(MPI_File_close(&fh));

    int verification = VERIFY_ERROR;
    if (rank == 0 && VERIFY_OUTPUT) {
        verification = verify_raw_file(path, nranks, size);
    }

    StratResult result;
    if (rank == 0) {
        result = summarize_runs(runs, NRUNS,
            static_cast<std::size_t>(nranks) * size,
            VERIFY_OUTPUT ? verification : -1);
    }

    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    if (rank == 0) std::remove(path);
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    return result;
}

} // namespace

int main(int argc, char** argv)
{
    std::setbuf(stdout, nullptr);
    std::setbuf(stderr, nullptr);

    int provided = 0;
    MPI_CHECK(MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided));

    int rank = 0;
    int nranks = 0;
    MPI_CHECK(MPI_Comm_rank(MPI_COMM_WORLD, &rank));
    MPI_CHECK(MPI_Comm_size(MPI_COMM_WORLD, &nranks));

    int device_count = 0;
    HIP_CHECK(hipGetDeviceCount(&device_count));
    HIP_CHECK(hipSetDevice(rank % device_count));

    if (rank == 0) {
        std::printf("MPI-IO Parallel Write Benchmark — MI250X / Lustre\n");
        std::printf("================================================\n");
        print_device_banner(device_count);
        print_run_config("MPI ranks", nranks, NRUNS);
        std::printf("  Lustre striping: filesystem layout set externally\n\n");
        print_strategy_legend(MPI_STRAT_INFO, MPI_DESCRIPTIONS, MPI_NSTRATS);
        print_allocation_legend();
        csv_write_header(CSV_FILE, benchmark_csv_header(MPI_STRAT_INFO, MPI_NSTRATS));
    }
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));

    for (std::size_t s = 0; s < N_BENCHMARK_SIZES; ++s) {
        const std::size_t size = BENCHMARK_SIZES[s];
        const std::string size_text = format_size(size);

        for (std::size_t a = 0; a < N_BENCHMARK_ALLOCS; ++a) {
            const AllocType alloc = BENCHMARK_ALLOCS[a];

            HostBuffer host{};
            const double local_d2h = run_d2h_transfer(
                rank, size, alloc, &host, DIRECT_IO_ALIGNMENT_BYTES);

            double wall_d2h = 0.0;
            MPI_CHECK(MPI_Allreduce(&local_d2h, &wall_d2h, 1,
                                    MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD));
            const double d2h_bw = (static_cast<double>(size) / 1e9) /
                                  (wall_d2h / 1e3);

            StratResult results[MPI_NSTRATS]{};
            for (int st = 0; st < MPI_NSTRATS; ++st) {
                const auto strat = static_cast<MPIWriteStrategy>(st);
                const std::string path = mpi_strat_file(strat, alloc);
                results[st] = benchmark_strategy(path.c_str(), rank, nranks,
                                                 host.data, size, strat);
            }

            if (rank == 0) {
                std::printf("%-8s  %-8s  D2H: %7.2f ms  %6.2f GB/s\n",
                            size_text.c_str(), ALLOC_INFO[alloc].name,
                            wall_d2h, d2h_bw);
                for (int st = 0; st < MPI_NSTRATS; ++st) {
                    print_strat_result(MPI_STRAT_INFO[st].name, results[st]);
                }
                std::printf("\n");
                csv_append_row(CSV_FILE,
                    benchmark_csv_row(size_text, alloc, wall_d2h, d2h_bw,
                                      results, MPI_STRAT_INFO, MPI_NSTRATS));
            }

            release_host_buffer(&host);
            MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
        }
    }

    if (rank == 0) std::printf("CSV written to: %s\n", CSV_FILE);
    MPI_CHECK(MPI_Finalize());
    return 0;
}
