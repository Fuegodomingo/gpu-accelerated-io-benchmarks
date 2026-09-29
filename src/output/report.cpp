/** @file report.cpp @brief Shared console reporting implementation. */

#include "output/report.h"

#include "benchmark/stats.h"
#include "validation/runtime_checks.h"
#include "validation/verify.h"

#include <cstdio>
#include <hip/hip_runtime.h>

void print_device_banner(int device_count)
{
    for (int d = 0; d < device_count; ++d) {
        hipDeviceProp_t prop{};
        HIP_CHECK(hipGetDeviceProperties(&prop, d));
        std::printf("  GCD %d : %s  (%.0f MB)\n", d, prop.name,
                    prop.totalGlobalMem / 1e6);
    }
}

void print_run_config(const char* participant_label, int participant_count, int nruns)
{
    std::printf("  %s : %d\n", participant_label, participant_count);
    std::printf("  NRUNS    : %d\n\n", nruns);
}

void print_strategy_legend(const EnumInfo* strategies, const char* const* descriptions,
                           int count)
{
    std::printf("  Strategies:\n");
    for (int i = 0; i < count; ++i) {
        std::printf("    %-20s — %s\n", strategies[i].name, descriptions[i]);
    }
    std::printf("\n");
}

void print_allocation_legend()
{
    std::printf("  Alloc types:\n");
    std::printf("    Pageable — malloc\n");
    std::printf("    Pinned   — hipHostMalloc\n");
    std::printf("    Managed  — hipMallocManaged + prefetch to CPU\n\n");
}

void print_strat_result(const char* label, const StratResult& result)
{
    std::printf("  %-20s  %8.2f ms  %6.2f GB/s  std %6.2f ms (%5.1f%%)  "
                "median %8.2f ms  %s\n",
                label, result.mean_ms, result.bw, result.std_ms,
                compute_std_percent(result.mean_ms, result.std_ms),
                result.median_ms, verify_status_str(result.ok));
}
