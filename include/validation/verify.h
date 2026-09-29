#pragma once

/**
 * @file verify.h
 * @brief Streaming correctness checks for raw and HDF5 benchmark outputs.
 */

#include <cstddef>

/** @brief Verification state. */
enum VerifyStatus {
    VERIFY_ERROR = -1,
    VERIFY_FAIL = 0,
    VERIFY_OK = 1
};

/** @brief Readable label for a verification state. */
const char* verify_status_str(int status);

/** @brief Verify ordered rank/device regions in a raw file without reading it all at once. */
int verify_raw_file(const char* path, int participants, std::size_t size_per_participant);

/** @brief Verify rank/device blocks whose order is unspecified, as in the mutex strategy. */
int verify_raw_file_unordered_blocks(const char* path, int participants,
                                     std::size_t size_per_participant);

/** @brief Serially reopen an HDF5 file and validate one run's float datasets. */
VerifyStatus verify_hdf5_file(const char* path, int nranks,
                              std::size_t size_per_rank, int run);

/** @brief Serially reopen an HDF5 file and validate stored double checksums. */
VerifyStatus verify_hdf5_double_file(const char* path, int nranks,
                                     std::size_t bytes_per_rank, int run);
