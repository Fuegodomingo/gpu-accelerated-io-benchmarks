/** @file stats.cpp @brief Statistical summaries for benchmark timings. */

#include "benchmark/stats.h"

#include <algorithm>
#include <cmath>
#include <vector>

StratResult summarize_runs(const double* times_ms, int n, std::size_t total_bytes,
                           int verification)
{
    StratResult result;
    result.ok = verification;
    if (!times_ms || n <= 0) return result;

    std::vector<double> sorted(times_ms, times_ms + n);
    std::sort(sorted.begin(), sorted.end());

    double sum = 0.0;
    for (double value : sorted) sum += value;
    result.mean_ms = sum / static_cast<double>(n);
    result.min_ms = sorted.front();
    result.max_ms = sorted.back();
    result.median_ms = (n % 2 == 0)
        ? 0.5 * (sorted[n / 2 - 1] + sorted[n / 2])
        : sorted[n / 2];

    double variance = 0.0;
    for (int i = 0; i < n; ++i) {
        const double delta = times_ms[i] - result.mean_ms;
        variance += delta * delta;
    }
    result.std_ms = n > 1 ? std::sqrt(variance / static_cast<double>(n - 1)) : 0.0;

    if (result.mean_ms > 0.0) {
        result.bw = (static_cast<double>(total_bytes) / 1e9) /
                    (result.mean_ms / 1e3);
    }
    return result;
}

double compute_std_percent(double mean_ms, double std_ms)
{
    return mean_ms > 0.0 ? 100.0 * std_ms / mean_ms : 0.0;
}
