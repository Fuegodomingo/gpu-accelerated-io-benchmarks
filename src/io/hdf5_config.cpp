/** @file hdf5_config.cpp @brief HDF5 benchmark configuration helpers. */

#include "io/hdf5_config.h"
#include "validation/runtime_checks.h"

#include <cstdio>
#include <cstdlib>

const char* getenv_default(const char* name, const char* fallback)
{
    const char* value = std::getenv(name);
    return value && value[0] != '\0' ? value : fallback;
}

MPI_Info create_hdf5_info(HDF5WriteStrategy strat)
{
    MPI_Info info{};
    MPI_CHECK(MPI_Info_create(&info));
    MPI_CHECK(MPI_Info_set(info, "access_style", "write_once"));
    MPI_CHECK(MPI_Info_set(info, "collective_buffering", "true"));
    MPI_CHECK(MPI_Info_set(info, "cb_buffer_size",
                           getenv_default("HDF5_CB_BUFFER_SIZE", "67108864")));
    MPI_CHECK(MPI_Info_set(info, "cb_block_size",
                           getenv_default("HDF5_CB_BLOCK_SIZE", "67108864")));

    if (strat == HDF5_STRAT_DIRECT) {
        MPI_CHECK(MPI_Info_set(info, "direct_io", "true"));
    }
    return info;
}

std::size_t hdf5_chunk_bytes()
{
    const char* value = std::getenv("HDF5_CHUNK_BYTES");
    if (value && value[0] != '\0') {
        return static_cast<std::size_t>(std::strtoull(value, nullptr, 10));
    }
    return 4ULL << 20;
}

void print_hdf5_hints(int rank)
{
    if (rank != 0) return;
    std::printf("  HDF5/ROMIO configuration:\n");
    std::printf("    Lustre striping        = filesystem layout (no MPI_Info override)\n");
    std::printf("    collective_buffering   = true\n");
    std::printf("    cb_buffer_size         = %s bytes\n",
                getenv_default("HDF5_CB_BUFFER_SIZE", "67108864"));
    std::printf("    cb_block_size          = %s bytes\n",
                getenv_default("HDF5_CB_BLOCK_SIZE", "67108864"));
    std::printf("    chunk_bytes            = %zu bytes\n", hdf5_chunk_bytes());
    std::printf("    direct alignment       = %zu bytes\n\n",
                DIRECT_IO_ALIGNMENT_BYTES);
}
