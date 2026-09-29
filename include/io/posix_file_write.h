#pragma once

/**
 * @file posix_file_write.h
 * @brief Multi-threaded POSIX write strategies used by the no-MPI benchmark.
 */

#include "benchmark/bench_params.h"
#include "benchmark/stats.h"
#include <cstddef>

/**
 * @brief Benchmark one POSIX strategy using one host buffer per GPU.
 * @param buffers Array of host-readable float buffers.
 * @param device_count Number of buffers/GPUs.
 * @param size Bytes in each device buffer.
 * @param alloc Allocation type, used only to construct unique filenames.
 * @param strat POSIX write strategy.
 */
StratResult benchmark_posix_strategy(float* const* buffers, int device_count,
                                     std::size_t size, AllocType alloc,
                                     PosixWriteStrategy strat);
