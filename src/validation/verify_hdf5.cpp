/** @file verify_hdf5.cpp @brief Streaming validation for HDF5 benchmark files. */

#include "validation/verify.h"

#include "io/hdf5_write.h"
#include "validation/checksum.h"

#include <algorithm>
#include <cmath>
#include <vector>
#include <hdf5.h>

namespace {

constexpr std::size_t VERIFY_CHUNK_BYTES = 64ULL << 20;

VerifyStatus verify_float_dataset(hid_t fid, int rank, std::size_t bytes, int run)
{
    char name[64];
    dataset_name(name, sizeof(name), rank, run);
    hid_t did = H5Dopen2(fid, name, H5P_DEFAULT);
    if (did < 0) return VERIFY_ERROR;

    const std::size_t elems_total = bytes / sizeof(float);
    const std::size_t elems_per_chunk = VERIFY_CHUNK_BYTES / sizeof(float);
    std::vector<float> buffer(std::min(elems_total, elems_per_chunk));
    hid_t file_space = H5Dget_space(did);
    if (file_space < 0) {
        H5Dclose(did);
        return VERIFY_ERROR;
    }

    std::size_t offset = 0;
    VerifyStatus status = VERIFY_OK;
    while (offset < elems_total) {
        const std::size_t n = std::min(elems_per_chunk, elems_total - offset);
        const hsize_t start[1] = {static_cast<hsize_t>(offset)};
        const hsize_t count[1] = {static_cast<hsize_t>(n)};
        if (H5Sselect_hyperslab(file_space, H5S_SELECT_SET, start,
                                nullptr, count, nullptr) < 0) {
            status = VERIFY_ERROR;
            break;
        }
        hid_t mem_space = H5Screate_simple(1, count, nullptr);
        if (mem_space < 0 || H5Dread(did, H5T_NATIVE_FLOAT, mem_space,
                                     file_space, H5P_DEFAULT, buffer.data()) < 0) {
            if (mem_space >= 0) H5Sclose(mem_space);
            status = VERIFY_ERROR;
            break;
        }
        H5Sclose(mem_space);
        for (std::size_t i = 0; i < n; ++i) {
            if (buffer[i] != static_cast<float>(rank)) {
                status = VERIFY_FAIL;
                break;
            }
        }
        if (status != VERIFY_OK) break;
        offset += n;
    }

    H5Sclose(file_space);
    H5Dclose(did);
    return status;
}

VerifyStatus checksum_double_dataset(hid_t fid, int rank, std::size_t bytes, int run)
{
    char name[64];
    dataset_name(name, sizeof(name), rank, run);
    hid_t did = H5Dopen2(fid, name, H5P_DEFAULT);
    if (did < 0) return VERIFY_ERROR;

    hid_t attr = H5Aopen(did, "checksum", H5P_DEFAULT);
    double expected = 0.0;
    if (attr < 0 || H5Aread(attr, H5T_NATIVE_DOUBLE, &expected) < 0) {
        if (attr >= 0) H5Aclose(attr);
        H5Dclose(did);
        return VERIFY_ERROR;
    }
    H5Aclose(attr);

    const std::size_t elems_total = bytes / sizeof(double);
    const std::size_t elems_per_chunk = VERIFY_CHUNK_BYTES / sizeof(double);
    std::vector<double> buffer(std::min(elems_total, elems_per_chunk));
    hid_t file_space = H5Dget_space(did);
    if (file_space < 0) {
        H5Dclose(did);
        return VERIFY_ERROR;
    }

    double actual = 0.0;
    std::size_t offset = 0;
    VerifyStatus status = VERIFY_OK;
    while (offset < elems_total) {
        const std::size_t n = std::min(elems_per_chunk, elems_total - offset);
        const hsize_t start[1] = {static_cast<hsize_t>(offset)};
        const hsize_t count[1] = {static_cast<hsize_t>(n)};
        if (H5Sselect_hyperslab(file_space, H5S_SELECT_SET, start,
                                nullptr, count, nullptr) < 0) {
            status = VERIFY_ERROR;
            break;
        }
        hid_t mem_space = H5Screate_simple(1, count, nullptr);
        if (mem_space < 0 || H5Dread(did, H5T_NATIVE_DOUBLE, mem_space,
                                     file_space, H5P_DEFAULT, buffer.data()) < 0) {
            if (mem_space >= 0) H5Sclose(mem_space);
            status = VERIFY_ERROR;
            break;
        }
        H5Sclose(mem_space);
        actual += compute_sum_checksum(buffer.data(), n);
        offset += n;
    }

    H5Sclose(file_space);
    H5Dclose(did);
    if (status != VERIFY_OK) return status;

    const double scale = std::max(1.0, std::fabs(expected));
    return std::fabs(actual - expected) <= 1e-9 * scale ? VERIFY_OK : VERIFY_FAIL;
}

} // namespace

VerifyStatus verify_hdf5_file(const char* path, int nranks,
                              std::size_t size_per_rank, int run)
{
    hid_t fid = H5Fopen(path, H5F_ACC_RDONLY, H5P_DEFAULT);
    if (fid < 0) return VERIFY_ERROR;

    VerifyStatus status = VERIFY_OK;
    for (int rank = 0; rank < nranks && status == VERIFY_OK; ++rank) {
        status = verify_float_dataset(fid, rank, size_per_rank, run);
    }
    H5Fclose(fid);
    return status;
}

VerifyStatus verify_hdf5_double_file(const char* path, int nranks,
                                     std::size_t bytes_per_rank, int run)
{
    hid_t fid = H5Fopen(path, H5F_ACC_RDONLY, H5P_DEFAULT);
    if (fid < 0) return VERIFY_ERROR;

    VerifyStatus status = VERIFY_OK;
    for (int rank = 0; rank < nranks && status == VERIFY_OK; ++rank) {
        status = checksum_double_dataset(fid, rank, bytes_per_rank, run);
    }
    H5Fclose(fid);
    return status;
}
