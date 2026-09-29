#pragma once

/**
 * @file hdf5_config.h
 * @brief HDF5/ROMIO configuration shared by HDF5 write paths.
 */

#include "benchmark/bench_params.h"
#include <cstddef>
#include <mpi.h>

/** @brief Read an environment variable with a fallback value. */
const char* getenv_default(const char* name, const char* fallback);

/**
 * @brief Build the MPI_Info used by the HDF5 MPI-IO file-access property list.
 *
 * Lustre stripe count/size are deliberately not overridden here; the benchmark
 * treats the filesystem layout created by lfs setstripe as the source of truth.
 */
MPI_Info create_hdf5_info(HDF5WriteStrategy strat);

/** @brief Chunk size for HDF5_STRAT_COLLECTIVE_CHUNKED. */
std::size_t hdf5_chunk_bytes();

/** @brief Print the HDF5/ROMIO settings relevant to a benchmark run. */
void print_hdf5_hints(int rank);
