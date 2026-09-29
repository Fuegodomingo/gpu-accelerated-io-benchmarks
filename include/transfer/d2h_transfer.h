#pragma once

/**
 * @file d2h_transfer.h
 * @brief Shared GPU fill and device-to-host transfer benchmark helpers.
 */

#include "benchmark/bench_params.h"
#include <cstddef>
#include <hip/hip_runtime.h>

/**
 * @brief Host allocation together with the original allocator-owned pointer.
 *
 * @c data may point inside @c base when an explicit alignment is requested;
 * releasing the allocation must therefore use release_host_buffer().
 */
struct HostBuffer {
    float* data = nullptr;
    void* base = nullptr;
    std::size_t size = 0;
    std::size_t allocation_size = 0;
    AllocType alloc = ALLOC_PAGEABLE;
};

/** @brief Fill a device buffer with a constant float value using a grid-stride loop. */
__global__ void fill_float_kernel(float* buf, float value, std::size_t n);

/** @brief Launch fill_float_kernel for a byte-sized float buffer. */
void launch_fill(float* d_buf, float value, std::size_t size, hipStream_t stream);

/**
 * @brief Fill a GPU buffer, copy it to host memory, and time D2H copies.
 * @param fill_value Value written into every float; ranks/device IDs use their ID.
 * @param size Buffer size in bytes.
 * @param alloc Host allocation strategy.
 * @param out Receives the host allocation and ownership metadata.
 * @param alignment Optional host-address alignment. Zero keeps allocator default.
 * @return Average D2H time in milliseconds over NRUNS copies.
 */
double run_d2h_transfer(int fill_value, std::size_t size, AllocType alloc,
                        HostBuffer* out, std::size_t alignment = 0);

/** @brief Release a HostBuffer returned by run_d2h_transfer(). */
void release_host_buffer(HostBuffer* buffer);

/** @brief Return true when the exposed data pointer satisfies an alignment. */
bool host_buffer_is_aligned(const HostBuffer& buffer, std::size_t alignment);
