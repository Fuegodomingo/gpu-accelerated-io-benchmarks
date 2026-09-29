/** @file d2h_transfer.cpp @brief HIP D2H transfer implementation. */

#include "transfer/d2h_transfer.h"

#include "benchmark/timer.h"
#include "validation/runtime_checks.h"

#include <cstdint>
#include <cstdlib>
#include <limits>

namespace {

std::uintptr_t align_up(std::uintptr_t address, std::size_t alignment)
{
    return ((address + alignment - 1) / alignment) * alignment;
}

void allocate_host_buffer(std::size_t size, AllocType alloc, std::size_t alignment,
                          HostBuffer* out)
{
    if (!out) std::abort();

    const std::size_t extra = alignment > 0 ? alignment - 1 : 0;
    if (size > std::numeric_limits<std::size_t>::max() - extra) {
        std::fprintf(stderr, "Host allocation size overflow\n");
        std::abort();
    }

    void* base = nullptr;
    const std::size_t allocation_size = size + extra;

    switch (alloc) {
        case ALLOC_PAGEABLE:
            base = std::malloc(allocation_size);
            if (!base) {
                std::fprintf(stderr, "malloc(%zu) failed\n", allocation_size);
                std::abort();
            }
            break;
        case ALLOC_PINNED:
            HIP_CHECK(hipHostMalloc(&base, allocation_size, hipHostMallocDefault));
            break;
        case ALLOC_MANAGED:
            HIP_CHECK(hipMallocManaged(&base, allocation_size, hipMemAttachGlobal));
            break;
        default:
            std::fprintf(stderr, "Unknown AllocType %d\n", static_cast<int>(alloc));
            std::abort();
    }

    void* exposed = base;
    if (alignment > 0) {
        exposed = reinterpret_cast<void*>(
            align_up(reinterpret_cast<std::uintptr_t>(base), alignment));
    }

    out->data = static_cast<float*>(exposed);
    out->base = base;
    out->size = size;
    out->allocation_size = allocation_size;
    out->alloc = alloc;
}

} // namespace

__global__ void fill_float_kernel(float* buf, float value, std::size_t n)
{
    std::size_t i = static_cast<std::size_t>(blockIdx.x) * blockDim.x + threadIdx.x;
    const std::size_t stride = static_cast<std::size_t>(blockDim.x) * gridDim.x;
    for (; i < n; i += stride) buf[i] = value;
}

void launch_fill(float* d_buf, float value, std::size_t size, hipStream_t stream)
{
    constexpr unsigned int block_size = 256;
    constexpr unsigned int max_blocks = 65535;

    const std::size_t n = size / sizeof(float);
    const std::size_t required_blocks = (n + block_size - 1) / block_size;
    const unsigned int grid = static_cast<unsigned int>(
        required_blocks < max_blocks ? required_blocks : max_blocks);

    fill_float_kernel<<<grid, block_size, 0, stream>>>(d_buf, value, n);
    HIP_CHECK(hipGetLastError());
}

double run_d2h_transfer(int fill_value, std::size_t size, AllocType alloc,
                        HostBuffer* out, std::size_t alignment)
{
    if (!out || size == 0 || size % sizeof(float) != 0) {
        std::fprintf(stderr, "run_d2h_transfer: invalid buffer arguments\n");
        std::abort();
    }

    hipStream_t stream{};
    HIP_CHECK(hipStreamCreate(&stream));

    float* d_buf = nullptr;
    HIP_CHECK(hipMalloc(&d_buf, size));
    launch_fill(d_buf, static_cast<float>(fill_value), size, stream);
    HIP_CHECK(hipStreamSynchronize(stream));

    allocate_host_buffer(size, alloc, alignment, out);

    HIP_CHECK(hipMemcpyAsync(out->data, d_buf, size, hipMemcpyDeviceToHost, stream));
    HIP_CHECK(hipStreamSynchronize(stream));

    if (alloc == ALLOC_MANAGED) {
        HIP_CHECK(hipMemPrefetchAsync(out->base, out->allocation_size,
                                      hipCpuDeviceId, stream));
        HIP_CHECK(hipStreamSynchronize(stream));
    }

    GpuTimer timer{};
    gpu_timer_start(&timer, stream);
    for (int i = 0; i < NRUNS; ++i) {
        HIP_CHECK(hipMemcpyAsync(out->data, d_buf, size,
                                 hipMemcpyDeviceToHost, stream));
    }
    const double total_ms = gpu_timer_stop_ms(&timer, stream);

    HIP_CHECK(hipStreamDestroy(stream));
    HIP_CHECK(hipFree(d_buf));
    return total_ms / static_cast<double>(NRUNS);
}

void release_host_buffer(HostBuffer* buffer)
{
    if (!buffer || !buffer->base) return;

    switch (buffer->alloc) {
        case ALLOC_PAGEABLE:
            std::free(buffer->base);
            break;
        case ALLOC_PINNED:
            HIP_CHECK(hipHostFree(buffer->base));
            break;
        case ALLOC_MANAGED:
            HIP_CHECK(hipFree(buffer->base));
            break;
        default:
            std::fprintf(stderr, "Unknown AllocType %d\n", static_cast<int>(buffer->alloc));
            std::abort();
    }

    *buffer = HostBuffer{};
}

bool host_buffer_is_aligned(const HostBuffer& buffer, std::size_t alignment)
{
    if (alignment == 0 || !buffer.data) return false;
    return reinterpret_cast<std::uintptr_t>(buffer.data) % alignment == 0;
}
