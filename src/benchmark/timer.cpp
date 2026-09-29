/** @file timer.cpp @brief Host and HIP timer implementations. */

#include "benchmark/timer.h"
#include "validation/runtime_checks.h"

#include <ctime>

double host_time_sec()
{
    timespec ts{};
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<double>(ts.tv_sec) + static_cast<double>(ts.tv_nsec) * 1e-9;
}

double host_elapsed_ms(double t0_sec, double t1_sec)
{
    return (t1_sec - t0_sec) * 1e3;
}

void gpu_timer_start(GpuTimer* timer, hipStream_t stream)
{
    HIP_CHECK(hipEventCreate(&timer->start));
    HIP_CHECK(hipEventCreate(&timer->stop));
    HIP_CHECK(hipEventRecord(timer->start, stream));
}

float gpu_timer_stop_ms(GpuTimer* timer, hipStream_t stream)
{
    HIP_CHECK(hipEventRecord(timer->stop, stream));
    HIP_CHECK(hipEventSynchronize(timer->stop));
    float elapsed_ms = 0.0f;
    HIP_CHECK(hipEventElapsedTime(&elapsed_ms, timer->start, timer->stop));
    HIP_CHECK(hipEventDestroy(timer->start));
    HIP_CHECK(hipEventDestroy(timer->stop));
    return elapsed_ms;
}
