#pragma once

/**
 * @file mpi_file_write.h
 * @brief Shared-file MPI-IO open and write helpers.
 */

#include "benchmark/bench_params.h"
#include <cstddef>
#include <mpi.h>

/**
 * @brief Collectively open and pre-size one shared MPI-IO output file.
 * @note Direct I/O adds the ROMIO direct_io=true hint; filesystem striping is
 *       intentionally left to lfs setstripe / the Slurm script.
 */
MPI_File open_shared_file(const char* path, int nranks, MPI_Offset size_per_rank,
                          MPIWriteStrategy strat);

/**
 * @brief Write one rank's buffer at offset rank*size.
 *
 * Independent and Direct-I/O strategies call MPI_File_write_at; Collective
 * calls MPI_File_write_at_all. All strategies share the same aligned chunk
 * loop for requests larger than the legacy MPI int-count limit.
 */
void write_data(MPI_File fh, const float* h_buf, std::size_t size, int rank,
                MPIWriteStrategy strat);
