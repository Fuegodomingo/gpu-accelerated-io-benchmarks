#pragma once

/**
 * @file csv.h
 * @brief CSV output helpers for benchmark tables.
 */

#include "benchmark/bench_params.h"
#include "benchmark/stats.h"
#include <string>

/** @brief Truncate a CSV file and write one header line. */
void csv_write_header(const char* filename, const std::string& header_line);

/** @brief Append one CSV row. */
void csv_append_row(const char* filename, const std::string& row_line);

/** @brief Build the common wide CSV header for an arbitrary strategy table. */
std::string benchmark_csv_header(const EnumInfo* strategies, int n_strategies);

/** @brief Build one common wide CSV row for a size/allocation cell. */
std::string benchmark_csv_row(const std::string& size_label, AllocType alloc,
                              double d2h_ms, double d2h_gbps,
                              const StratResult* results,
                              const EnumInfo* strategies, int n_strategies);
