#pragma once

/**
 * @file hdf5_write.h
 * @brief Parallel HDF5 file, dataset, transfer-property, and write helpers.
 */

#include "benchmark/bench_params.h"
#include <cstddef>
#include <hdf5.h>

/** @brief Collectively create an HDF5 file configured for one write strategy. */
hid_t open_hdf5(const char* path, HDF5WriteStrategy strat);

/** @brief Build /rank_<rank>_run_<run>. */
void dataset_name(char* out, std::size_t out_size, int rank, int run);

/**
 * @brief Collectively create all per-rank datasets for one run and return this rank's handle.
 * @param out_dcpl Receives an open dataset-creation property list owned by the caller.
 */
hid_t create_dataset_collective(hid_t fid, hsize_t n_elems, hid_t elem_type,
                                int rank, int nranks, int run,
                                HDF5WriteStrategy strat, hid_t* out_dcpl);

/** @brief Create independent/collective HDF5 dataset-transfer properties. */
hid_t create_hdf5_dxpl(HDF5WriteStrategy strat);

/**
 * @brief Write an existing float dataset.
 * @note Direct I/O uses explicit aligned hyperslab splitting; other strategies
 *       issue one H5Dwrite and let HDF5/MPI-IO manage the transfer.
 */
void write_hdf5_dataset_float(hid_t did, hid_t dxpl, const float* h_buf,
                              std::size_t bytes, HDF5WriteStrategy strat);

/** @brief Write an existing double dataset. */
void write_hdf5_dataset_double(hid_t did, hid_t dxpl, const double* h_buf);

/** @brief Create, write, and close one float dataset. */
void write_hdf5(hid_t fid, const float* h_buf, std::size_t bytes,
                int rank, int nranks, int run, HDF5WriteStrategy strat);

/** @brief Create, write, checksum, and close one double dataset. */
void write_hdf5_double(hid_t fid, const double* h_buf, std::size_t bytes,
                       int rank, int nranks, int run, HDF5WriteStrategy strat);

/** @brief Return true for the two collective HDF5 transfer strategies. */
bool hdf5_strategy_is_collective(HDF5WriteStrategy strat);
