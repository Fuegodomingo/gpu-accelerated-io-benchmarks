#pragma once

/**
 * @file stats.h
 * @brief Statistics shared by all benchmark front-ends.
 */

#include <cstddef>

/** @brief Summary of one write strategy over all timed runs. */
struct StratResult {
    double mean_ms = 0.0;
    double std_ms = 0.0;
    double median_ms = 0.0;
    double min_ms = 0.0;
    double max_ms = 0.0;
    double bw = 0.0; /**< Aggregate GB/s computed from total_bytes / mean_ms. */
    int ok = -1;     /**< 1 verified, 0 mismatch, -1 not checked/error. */
};

/**
 * @brief Summarize timing samples without changing the historical bandwidth definition.
 * @param times_ms Timing samples in milliseconds.
 * @param n Number of samples.
 * @param total_bytes Aggregate bytes written by all participants per sample.
 * @param verification Verification state copied into the result.
 */
StratResult summarize_runs(const double* times_ms, int n, std::size_t total_bytes,
                           int verification = -1);

/** @brief Coefficient of variation as a percentage of the mean. */
double compute_std_percent(double mean_ms, double std_ms);
