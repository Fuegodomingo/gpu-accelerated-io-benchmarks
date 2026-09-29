#pragma once

/**
 * @file runtime_checks.h
 * @brief Lightweight error-checking macros for HIP, MPI, and HDF5 calls.
 */

#include <cstdio>
#include <cstdlib>
#include <hip/hip_runtime.h>

#define HIP_CHECK(call)                                                          \
    do {                                                                         \
        const hipError_t _err = (call);                                          \
        if (_err != hipSuccess) {                                                \
            std::fprintf(stderr, "HIP error at %s:%d — %s\n",                  \
                         __FILE__, __LINE__, hipGetErrorString(_err));            \
            std::abort();                                                        \
        }                                                                        \
    } while (0)

/* The translation unit using MPI_CHECK must include <mpi.h>. */
#define MPI_CHECK(call)                                                          \
    do {                                                                         \
        const int _err = (call);                                                 \
        if (_err != MPI_SUCCESS) {                                               \
            char _msg[MPI_MAX_ERROR_STRING];                                     \
            int _len = 0;                                                        \
            MPI_Error_string(_err, _msg, &_len);                                \
            std::fprintf(stderr, "MPI error at %s:%d — %.*s\n",                \
                         __FILE__, __LINE__, _len, _msg);                         \
            MPI_Abort(MPI_COMM_WORLD, _err);                                     \
        }                                                                        \
    } while (0)

#define HDF5_CHECK(call)                                                         \
    do {                                                                         \
        const auto _status = (call);                                             \
        if (_status < 0) {                                                       \
            std::fprintf(stderr, "HDF5 error at %s:%d\n", __FILE__, __LINE__); \
            std::abort();                                                        \
        }                                                                        \
    } while (0)
