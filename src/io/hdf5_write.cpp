/** @file hdf5_write.cpp @brief Parallel HDF5 write implementation. */

#include "io/hdf5_write.h"

#include "io/hdf5_config.h"
#include "validation/checksum.h"
#include "validation/runtime_checks.h"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstdio>

namespace {

std::size_t largest_direct_request()
{
    return (static_cast<std::size_t>(INT_MAX) / DIRECT_IO_ALIGNMENT_BYTES) *
           DIRECT_IO_ALIGNMENT_BYTES;
}

void write_hdf5_dataset_float_direct(hid_t did, hid_t dxpl,
                                     const float* h_buf, std::size_t total_bytes)
{
    const std::size_t max_request = largest_direct_request();
    hid_t file_space = H5Dget_space(did);
    HDF5_CHECK(file_space);

    std::size_t byte_offset = 0;
    while (byte_offset < total_bytes) {
        const std::size_t remaining = total_bytes - byte_offset;
        const std::size_t chunk_bytes = std::min(remaining, max_request);
        const hsize_t start[1] = {static_cast<hsize_t>(byte_offset / sizeof(float))};
        const hsize_t count[1] = {static_cast<hsize_t>(chunk_bytes / sizeof(float))};

        HDF5_CHECK(H5Sselect_hyperslab(file_space, H5S_SELECT_SET, start,
                                       nullptr, count, nullptr));
        hid_t mem_space = H5Screate_simple(1, count, nullptr);
        HDF5_CHECK(mem_space);

        const void* chunk_ptr = reinterpret_cast<const char*>(h_buf) + byte_offset;
        HDF5_CHECK(H5Dwrite(did, H5T_NATIVE_FLOAT, mem_space, file_space,
                            dxpl, chunk_ptr));
        HDF5_CHECK(H5Sclose(mem_space));
        byte_offset += chunk_bytes;
    }

    HDF5_CHECK(H5Sclose(file_space));
}

void attach_checksum_double(hid_t did, const double* h_buf, std::size_t n_doubles)
{
    const double checksum = compute_sum_checksum(h_buf, n_doubles);
    hid_t space = H5Screate(H5S_SCALAR);
    HDF5_CHECK(space);
    hid_t attr = H5Acreate2(did, "checksum", H5T_NATIVE_DOUBLE, space,
                            H5P_DEFAULT, H5P_DEFAULT);
    HDF5_CHECK(attr);
    HDF5_CHECK(H5Awrite(attr, H5T_NATIVE_DOUBLE, &checksum));
    HDF5_CHECK(H5Aclose(attr));
    HDF5_CHECK(H5Sclose(space));
}

} // namespace

hid_t open_hdf5(const char* path, HDF5WriteStrategy strat)
{
    MPI_Info info = create_hdf5_info(strat);

    hid_t fapl = H5Pcreate(H5P_FILE_ACCESS);
    HDF5_CHECK(fapl);
    HDF5_CHECK(H5Pset_fapl_mpio(fapl, MPI_COMM_WORLD, info));
    HDF5_CHECK(H5Pset_all_coll_metadata_ops(fapl, true));
    HDF5_CHECK(H5Pset_coll_metadata_write(fapl, true));
    HDF5_CHECK(H5Pset_libver_bounds(fapl, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST));

    if (strat == HDF5_STRAT_DIRECT) {
        HDF5_CHECK(H5Pset_alignment(fapl, DIRECT_IO_ALIGNMENT_BYTES,
                                    DIRECT_IO_ALIGNMENT_BYTES));
    }

    MPI_CHECK(MPI_Info_free(&info));
    hid_t fid = H5Fcreate(path, H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    HDF5_CHECK(fid);
    HDF5_CHECK(H5Pclose(fapl));
    return fid;
}

void dataset_name(char* out, std::size_t out_size, int rank, int run)
{
    std::snprintf(out, out_size, "/rank_%d_run_%d", rank, run);
}

hid_t create_dataset_collective(hid_t fid, hsize_t n_elems, hid_t elem_type,
                                int rank, int nranks, int run,
                                HDF5WriteStrategy strat, hid_t* out_dcpl)
{
    hid_t sid = H5Screate_simple(1, &n_elems, nullptr);
    HDF5_CHECK(sid);

    hid_t dcpl = H5Pcreate(H5P_DATASET_CREATE);
    HDF5_CHECK(dcpl);
    HDF5_CHECK(H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_EARLY));

    if (strat == HDF5_STRAT_COLLECTIVE_CHUNKED) {
        const std::size_t elem_size = H5Tget_size(elem_type);
        const std::size_t requested = std::max<std::size_t>(1, hdf5_chunk_bytes() / elem_size);
        const hsize_t chunk[1] = {
            std::min<hsize_t>(n_elems, static_cast<hsize_t>(requested))
        };
        HDF5_CHECK(H5Pset_chunk(dcpl, 1, chunk));
    }

    hid_t dapl = H5Pcreate(H5P_DATASET_ACCESS);
    HDF5_CHECK(dapl);
    HDF5_CHECK(H5Pset_chunk_cache(dapl, H5D_CHUNK_CACHE_NSLOTS_DEFAULT,
                                  0, H5D_CHUNK_CACHE_W0_DEFAULT));

    hid_t my_did = -1;
    for (int r = 0; r < nranks; ++r) {
        char name[64];
        dataset_name(name, sizeof(name), r, run);
        hid_t did = H5Dcreate2(fid, name, elem_type, sid, H5P_DEFAULT, dcpl, dapl);
        HDF5_CHECK(did);
        if (r == rank) my_did = did;
        else HDF5_CHECK(H5Dclose(did));
    }

    HDF5_CHECK(H5Pclose(dapl));
    HDF5_CHECK(H5Sclose(sid));
    *out_dcpl = dcpl;
    return my_did;
}

hid_t create_hdf5_dxpl(HDF5WriteStrategy strat)
{
    hid_t dxpl = H5Pcreate(H5P_DATASET_XFER);
    HDF5_CHECK(dxpl);
    const H5FD_mpio_xfer_t mode = hdf5_strategy_is_collective(strat)
        ? H5FD_MPIO_COLLECTIVE
        : H5FD_MPIO_INDEPENDENT;
    HDF5_CHECK(H5Pset_dxpl_mpio(dxpl, mode));
    return dxpl;
}

void write_hdf5_dataset_float(hid_t did, hid_t dxpl, const float* h_buf,
                              std::size_t bytes, HDF5WriteStrategy strat)
{
    if (strat == HDF5_STRAT_DIRECT) {
        write_hdf5_dataset_float_direct(did, dxpl, h_buf, bytes);
    } else {
        HDF5_CHECK(H5Dwrite(did, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, dxpl, h_buf));
    }
}

void write_hdf5_dataset_double(hid_t did, hid_t dxpl, const double* h_buf)
{
    HDF5_CHECK(H5Dwrite(did, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, dxpl, h_buf));
}

void write_hdf5(hid_t fid, const float* h_buf, std::size_t bytes,
                int rank, int nranks, int run, HDF5WriteStrategy strat)
{
    hid_t dcpl{};
    hid_t did = create_dataset_collective(fid, bytes / sizeof(float),
                                          H5T_NATIVE_FLOAT, rank, nranks,
                                          run, strat, &dcpl);
    hid_t dxpl = create_hdf5_dxpl(strat);
    write_hdf5_dataset_float(did, dxpl, h_buf, bytes, strat);
    HDF5_CHECK(H5Pclose(dxpl));
    HDF5_CHECK(H5Dclose(did));
    HDF5_CHECK(H5Pclose(dcpl));
}

void write_hdf5_double(hid_t fid, const double* h_buf, std::size_t bytes,
                       int rank, int nranks, int run, HDF5WriteStrategy strat)
{
    hid_t dcpl{};
    hid_t did = create_dataset_collective(fid, bytes / sizeof(double),
                                          H5T_NATIVE_DOUBLE, rank, nranks,
                                          run, strat, &dcpl);
    hid_t dxpl = create_hdf5_dxpl(strat);
    write_hdf5_dataset_double(did, dxpl, h_buf);
    attach_checksum_double(did, h_buf, bytes / sizeof(double));
    HDF5_CHECK(H5Pclose(dxpl));
    HDF5_CHECK(H5Dclose(did));
    HDF5_CHECK(H5Pclose(dcpl));
}

bool hdf5_strategy_is_collective(HDF5WriteStrategy strat)
{
    return strat == HDF5_STRAT_COLLECTIVE ||
           strat == HDF5_STRAT_COLLECTIVE_CHUNKED;
}
