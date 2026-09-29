#pragma once

/**
 * @file bench_params.h
 * @brief Shared benchmark enums, metadata, sizes, and filename helpers.
 */

#include <cstddef>
#include <string>

/** @brief Number of timed iterations per benchmark configuration. */
inline constexpr int NRUNS = 10;

/**
 * @brief Alignment used for Direct-I/O request splitting and HDF5 allocation.
 *
 * Four MiB matches the request alignment used by the diagnostic benchmark that
 * avoided the pathological INT_MAX split for large Direct-I/O transfers.
 */
inline constexpr std::size_t DIRECT_IO_ALIGNMENT_BYTES = 4ULL << 20;

/** @brief Host-memory allocation strategy for D2H destination buffers. */
enum AllocType {
    ALLOC_PAGEABLE = 0,
    ALLOC_PINNED,
    ALLOC_MANAGED,
    NALLOCS
};

/** @brief Display and file/CSV metadata associated with one enum value. */
struct EnumInfo {
    const char* name;       /**< Readable label. */
    const char* tag;        /**< Filename-safe tag. */
    const char* csv_prefix; /**< Short prefix for wide CSV columns. */
};

inline constexpr EnumInfo ALLOC_INFO[NALLOCS] = {
    {"Pageable", "pageable", "Pageable"},
    {"Pinned",   "pinned",   "Pinned"},
    {"Managed",  "managed",  "Managed"}
};

/** @brief MPI-IO write strategy. */
enum MPIWriteStrategy {
    MPI_STRAT_INDEPENDENT = 0,
    MPI_STRAT_COLLECTIVE,
    MPI_STRAT_DIRECT,
    MPI_NSTRATS
};

inline constexpr EnumInfo MPI_STRAT_INFO[MPI_NSTRATS] = {
    {"Independent", "independent", "Ind"},
    {"Collective",  "collective",  "Col"},
    {"Direct I/O",  "direct",      "Dir"}
};

/** @brief HDF5 write strategy. */
enum HDF5WriteStrategy {
    HDF5_STRAT_INDEPENDENT = 0,
    HDF5_STRAT_COLLECTIVE,
    HDF5_STRAT_COLLECTIVE_CHUNKED,
    HDF5_STRAT_DIRECT,
    HDF5_NSTRATS
};

inline constexpr EnumInfo HDF5_STRAT_INFO[HDF5_NSTRATS] = {
    {"Independent",        "independent",        "Ind"},
    {"Collective",         "collective",         "Col"},
    {"Collective Chunked", "collective_chunked", "Chk"},
    {"Direct I/O",         "direct",             "Dir"}
};

/** @brief POSIX multi-GPU write strategy. */
enum PosixWriteStrategy {
    POSIX_STRAT_INDIVIDUAL_MERGE = 0,
    POSIX_STRAT_PWRITE,
    POSIX_STRAT_MUTEX,
    POSIX_NSTRATS
};

inline constexpr EnumInfo POSIX_STRAT_INFO[POSIX_NSTRATS] = {
    {"Individual + merge", "individual", "Ind"},
    {"pwrite offsets",     "pwrite",     "PWrite"},
    {"Mutex",              "mutex",      "Mutex"}
};

/**
 * @brief Canonical size sweep used by the MPI/HDF5 benchmarks.
 *
 * Edit this one table when a run needs a different sweep.
 */
inline constexpr std::size_t BENCHMARK_SIZES[] = {
    1ULL   << 20,
    64ULL  << 20,
    256ULL << 20,
    1ULL   << 30,
    2ULL   << 30,
    4ULL   << 30,
    8ULL   << 30
};

inline constexpr std::size_t N_BENCHMARK_SIZES =
    sizeof(BENCHMARK_SIZES) / sizeof(BENCHMARK_SIZES[0]);

/** @brief Whether expensive read-back verification is enabled for MPI/HDF5 runs. */
inline constexpr bool VERIFY_OUTPUT = false;

/** @brief Allocation strategies enabled by default. */
inline constexpr AllocType BENCHMARK_ALLOCS[] = {
    ALLOC_PAGEABLE,
    ALLOC_PINNED,
    ALLOC_MANAGED
};

inline constexpr std::size_t N_BENCHMARK_ALLOCS =
    sizeof(BENCHMARK_ALLOCS) / sizeof(BENCHMARK_ALLOCS[0]);

/** @brief Build an MPI benchmark output filename. */
std::string mpi_strat_file(MPIWriteStrategy strat, AllocType alloc);

/** @brief Build an HDF5 benchmark output filename. */
std::string hdf5_strat_file(HDF5WriteStrategy strat, AllocType alloc);

/** @brief Build a POSIX benchmark output filename. */
std::string posix_strat_file(PosixWriteStrategy strat, AllocType alloc);

/** @brief Format a binary byte count using the benchmark's historical MB/GB labels. */
std::string format_size(std::size_t size_bytes);
