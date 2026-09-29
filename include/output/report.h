#pragma once

/**
 * @file report.h
 * @brief Console reporting shared across benchmark executables.
 */

#include "benchmark/bench_params.h"
#include "benchmark/stats.h"

/** @brief Print visible GPU/GCD information. */
void print_device_banner(int device_count);

/** @brief Print participant and run-count configuration. */
void print_run_config(const char* participant_label, int participant_count, int nruns);

/** @brief Print an enum-based strategy legend with caller-supplied descriptions. */
void print_strategy_legend(const EnumInfo* strategies, const char* const* descriptions,
                           int count);

/** @brief Print the three host allocation strategies. */
void print_allocation_legend();

/** @brief Print one benchmark result line. */
void print_strat_result(const char* label, const StratResult& result);
