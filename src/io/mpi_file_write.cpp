/** @file mpi_file_write.cpp @brief MPI-IO file lifecycle and writes. */

#include "io/mpi_file_write.h"
#include "validation/runtime_checks.h"

#include <climits>
#include <cstdio>

namespace {

int largest_aligned_chunk()
{
    return static_cast<int>(
        (static_cast<std::size_t>(INT_MAX) / DIRECT_IO_ALIGNMENT_BYTES) *
        DIRECT_IO_ALIGNMENT_BYTES);
}

} // namespace

MPI_File open_shared_file(const char* path, int nranks, MPI_Offset size_per_rank,
                          MPIWriteStrategy strat)
{
    MPI_Info info{};
    MPI_CHECK(MPI_Info_create(&info));
    MPI_CHECK(MPI_Info_set(info, "access_style", "write_once"));
    MPI_CHECK(MPI_Info_set(info, "collective_buffering", "true"));
    MPI_CHECK(MPI_Info_set(info, "cb_buffer_size", "67108864"));
    MPI_CHECK(MPI_Info_set(info, "cb_block_size", "67108864"));

    if (strat == MPI_STRAT_DIRECT) {
        MPI_CHECK(MPI_Info_set(info, "direct_io", "true"));
    }

    MPI_File fh{};
    MPI_CHECK(MPI_File_open(MPI_COMM_WORLD, path,
                            MPI_MODE_CREATE | MPI_MODE_WRONLY, info, &fh));
    MPI_CHECK(MPI_File_set_size(fh,
        static_cast<MPI_Offset>(nranks) * size_per_rank));
    MPI_CHECK(MPI_Info_free(&info));
    return fh;
}

void write_data(MPI_File fh, const float* h_buf, std::size_t size, int rank,
                MPIWriteStrategy strat)
{
    MPI_Offset offset = static_cast<MPI_Offset>(rank) * static_cast<MPI_Offset>(size);
    MPI_Offset remaining = static_cast<MPI_Offset>(size);
    const char* src = reinterpret_cast<const char*>(h_buf);
    const int max_chunk = largest_aligned_chunk();

    while (remaining > 0) {
        const int chunk = remaining > static_cast<MPI_Offset>(max_chunk)
            ? max_chunk
            : static_cast<int>(remaining);

        MPI_Status status{};
        if (strat == MPI_STRAT_COLLECTIVE) {
            MPI_CHECK(MPI_File_write_at_all(fh, offset, src, chunk, MPI_BYTE, &status));
        } else {
            MPI_CHECK(MPI_File_write_at(fh, offset, src, chunk, MPI_BYTE, &status));
        }

        int written = 0;
        MPI_CHECK(MPI_Get_count(&status, MPI_BYTE, &written));
        if (written <= 0) {
            std::fprintf(stderr, "MPI-IO write made no progress\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        remaining -= written;
        offset += written;
        src += written;
    }
}
