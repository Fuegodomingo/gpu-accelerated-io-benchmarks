/**
 * @file hdf5_main.cpp
 * @brief Unified parallel HDF5 benchmark, including the Direct-I/O path.
 *
 * Independent, Collective, Collective+Chunked, and Direct I/O share the same
 * benchmark loop. Strategy-specific behavior lives in hdf5_write.cpp.
 */

#include "benchmark/bench_params.h"
#include "benchmark/stats.h"
#include "benchmark/mpi_timer.h"
#include "io/hdf5_config.h"
#include "io/hdf5_write.h"
#include "output/csv.h"
#include "output/report.h"
#include "transfer/d2h_transfer.h"
#include "validation/runtime_checks.h"
#include "validation/verify.h"

#include <cstdio>
#include <cstdint>
#include <hdf5.h>
#include <mpi.h>

namespace {

constexpr const char* CSV_FILE = "hdf5_bench.csv";

const char* const HDF5_DESCRIPTIONS[HDF5_NSTRATS] = {
    "H5FD_MPIO_INDEPENDENT",
    "H5FD_MPIO_COLLECTIVE, contiguous dataset",
    "H5FD_MPIO_COLLECTIVE, chunked dataset",
    "H5FD_MPIO_INDEPENDENT + direct_io + aligned HDF5 allocations"
};

/** @brief Warn if a requested collective HDF5 write fell back to independent I/O. */
void check_collective_mode(hid_t dxpl, HDF5WriteStrategy strat, int rank)
{
    if (!hdf5_strategy_is_collective(strat)) return;

    H5D_mpio_actual_io_mode_t mode{};
    HDF5_CHECK(H5Pget_mpio_actual_io_mode(dxpl, &mode));
    if (mode == H5D_MPIO_CONTIGUOUS_COLLECTIVE || mode == H5D_MPIO_CHUNK_COLLECTIVE)
        return;

    uint32_t local_cause = 0;
    uint32_t global_cause = 0;
    HDF5_CHECK(H5Pget_mpio_no_collective_cause(dxpl, &local_cause, &global_cause));
    std::fprintf(stderr,
        "[rank %d] %s fell back from collective I/O "
        "(local_cause=0x%x, global_cause=0x%x)\n",
        rank, HDF5_STRAT_INFO[strat].name, local_cause, global_cause);
}

/** @brief Benchmark one HDF5 strategy with pre-created datasets and timed flushes. */
StratResult benchmark_strategy(const char* path, int rank, int nranks,
                               const float* h_buf, std::size_t size,
                               HDF5WriteStrategy strat)
{
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    hid_t fid = open_hdf5(path, strat);
    const hsize_t n_floats = size / sizeof(float);

    hid_t dxpl = create_hdf5_dxpl(strat);

    hid_t warmup_dcpl{};
    hid_t warmup = create_dataset_collective(fid, n_floats, H5T_NATIVE_FLOAT,
                                             rank, nranks, -1, strat,
                                             &warmup_dcpl);
    HDF5_CHECK(H5Pclose(warmup_dcpl));
    write_hdf5_dataset_float(warmup, dxpl, h_buf, size, strat);
    HDF5_CHECK(H5Fflush(fid, H5F_SCOPE_GLOBAL));
    HDF5_CHECK(H5Dclose(warmup));
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));

    hid_t datasets[NRUNS]{};
    for (int r = 0; r < NRUNS; ++r) {
        hid_t dcpl{};
        datasets[r] = create_dataset_collective(fid, n_floats, H5T_NATIVE_FLOAT,
                                                rank, nranks, r, strat, &dcpl);
        HDF5_CHECK(H5Pclose(dcpl));
    }
    HDF5_CHECK(H5Fflush(fid, H5F_SCOPE_GLOBAL));
    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));

    double runs[NRUNS]{};
    for (int r = 0; r < NRUNS; ++r) {
        const double t0 = mpi_region_begin(MPI_COMM_WORLD);
        write_hdf5_dataset_float(datasets[r], dxpl, h_buf, size, strat);
        HDF5_CHECK(H5Fflush(fid, H5F_SCOPE_GLOBAL));
        runs[r] = mpi_region_end_ms(t0, MPI_COMM_WORLD);
        if (r == 0) check_collective_mode(dxpl, strat, rank);
    }

    for (hid_t dataset : datasets) HDF5_CHECK(H5Dclose(dataset));
    HDF5_CHECK(H5Pclose(dxpl));
    HDF5_CHECK(H5Fclose(fid));

    MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
    int verification = -1;
    if (rank == 0 && VERIFY_OUTPUT) {
        verification = verify_hdf5_file(path, nranks, size, 0);
    }

    StratResult result;
    if (rank == 0) {
        result = summarize_runs(runs, NRUNS,
            static_cast<std::size_t>(nranks) * size,
            VERIFY_OUTPUT ? verification : -1);
        std::remove(path);
    }
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
        std::printf("HDF5 Parallel Write Benchmark — MI250X / Lustre\n");
        std::printf("================================================\n");
        print_device_banner(device_count);
        print_run_config("MPI ranks", nranks, NRUNS);
        print_strategy_legend(HDF5_STRAT_INFO, HDF5_DESCRIPTIONS, HDF5_NSTRATS);
        print_allocation_legend();
        print_hdf5_hints(rank);
        csv_write_header(CSV_FILE, benchmark_csv_header(HDF5_STRAT_INFO, HDF5_NSTRATS));
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
            if (!host_buffer_is_aligned(host, DIRECT_IO_ALIGNMENT_BYTES)) {
                std::fprintf(stderr, "Direct-I/O host alignment failed on rank %d\n", rank);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }

            double wall_d2h = 0.0;
            MPI_CHECK(MPI_Allreduce(&local_d2h, &wall_d2h, 1,
                                    MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD));
            const double d2h_bw = (static_cast<double>(size) / 1e9) /
                                  (wall_d2h / 1e3);

            StratResult results[HDF5_NSTRATS]{};
            for (int st = 0; st < HDF5_NSTRATS; ++st) {
                const auto strat = static_cast<HDF5WriteStrategy>(st);
                const std::string path = hdf5_strat_file(strat, alloc);
                results[st] = benchmark_strategy(path.c_str(), rank, nranks,
                                                 host.data, size, strat);
            }

            if (rank == 0) {
                std::printf("%-8s  %-8s  D2H: %7.2f ms  %6.2f GB/s\n",
                            size_text.c_str(), ALLOC_INFO[alloc].name,
                            wall_d2h, d2h_bw);
                for (int st = 0; st < HDF5_NSTRATS; ++st) {
                    print_strat_result(HDF5_STRAT_INFO[st].name, results[st]);
                }
                std::printf("\n");
                csv_append_row(CSV_FILE,
                    benchmark_csv_row(size_text, alloc, wall_d2h, d2h_bw,
                                      results, HDF5_STRAT_INFO, HDF5_NSTRATS));
            }

            release_host_buffer(&host);
            MPI_CHECK(MPI_Barrier(MPI_COMM_WORLD));
        }
    }

    if (rank == 0) std::printf("CSV written to: %s\n", CSV_FILE);
    MPI_CHECK(MPI_Finalize());
    return 0;
}
