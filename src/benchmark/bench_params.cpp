/** @file bench_params.cpp @brief Implementations for benchmark parameter helpers. */

#include "benchmark/bench_params.h"

#include <sstream>

std::string mpi_strat_file(MPIWriteStrategy strat, AllocType alloc)
{
    return "mpi_" + std::string(MPI_STRAT_INFO[strat].tag) + "_" +
           ALLOC_INFO[alloc].tag + ".bin";
}

std::string hdf5_strat_file(HDF5WriteStrategy strat, AllocType alloc)
{
    return "hdf5_" + std::string(HDF5_STRAT_INFO[strat].tag) + "_" +
           ALLOC_INFO[alloc].tag + ".h5";
}

std::string posix_strat_file(PosixWriteStrategy strat, AllocType alloc)
{
    return "posix_" + std::string(POSIX_STRAT_INFO[strat].tag) + "_" +
           ALLOC_INFO[alloc].tag + ".bin";
}

std::string format_size(std::size_t size_bytes)
{
    std::ostringstream out;
    if (size_bytes >= (1ULL << 30) && size_bytes % (1ULL << 30) == 0) {
        out << (size_bytes >> 30) << " GB";
    } else if (size_bytes >= (1ULL << 20) && size_bytes % (1ULL << 20) == 0) {
        out << (size_bytes >> 20) << " MB";
    } else if (size_bytes >= (1ULL << 10) && size_bytes % (1ULL << 10) == 0) {
        out << (size_bytes >> 10) << " KB";
    } else {
        out << size_bytes << " B";
    }
    return out.str();
}
