#pragma once

/**
 * @file timer.h
 * @brief Host and HIP-event timers usable by both MPI and non-MPI benchmarks.
 */

#include <hip/hip_runtime.h>

/** @brief Return a monotonic host timestamp in seconds. */
double host_time_sec();

/** @brief Return milliseconds between two host timestamps. */
double host_elapsed_ms(double t0_sec, double t1_sec);

/** @brief Single-use HIP event timer. */
struct GpuTimer {
    hipEvent_t start{};
    hipEvent_t stop{};
};

/** @brief Create events and record the start event on a HIP stream. */
void gpu_timer_start(GpuTimer* timer, hipStream_t stream);

/** @brief Record stop, synchronize, return elapsed ms, and destroy events. */
float gpu_timer_stop_ms(GpuTimer* timer, hipStream_t stream);
