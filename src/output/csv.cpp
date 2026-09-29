/** @file csv.cpp @brief CSV output implementation. */

#include "output/csv.h"

#include "benchmark/stats.h"
#include "validation/verify.h"

#include <cstdio>
#include <iomanip>
#include <sstream>

void csv_write_header(const char* filename, const std::string& header_line)
{
    FILE* file = std::fopen(filename, "w");
    if (!file) {
        std::fprintf(stderr, "Warning: could not open %s for writing\n", filename);
        return;
    }
    std::fprintf(file, "%s\n", header_line.c_str());
    std::fclose(file);
}

void csv_append_row(const char* filename, const std::string& row_line)
{
    FILE* file = std::fopen(filename, "a");
    if (!file) {
        std::fprintf(stderr, "Warning: could not open %s for appending\n", filename);
        return;
    }
    std::fprintf(file, "%s\n", row_line.c_str());
    std::fclose(file);
}

std::string benchmark_csv_header(const EnumInfo* strategies, int n_strategies)
{
    std::ostringstream out;
    out << "Size,Alloc,D2H_ms,D2H_GB/s";
    for (int i = 0; i < n_strategies; ++i) {
        const char* p = strategies[i].csv_prefix;
        out << ',' << p << "_mean_ms"
            << ',' << p << "_GB/s"
            << ',' << p << "_std_ms"
            << ',' << p << "_std_pct"
            << ',' << p << "_median_ms"
            << ',' << p << "_min_ms"
            << ',' << p << "_max_ms"
            << ',' << p << "_OK";
    }
    return out.str();
}

std::string benchmark_csv_row(const std::string& size_label, AllocType alloc,
                              double d2h_ms, double d2h_gbps,
                              const StratResult* results,
                              const EnumInfo* /*strategies*/, int n_strategies)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(2)
        << size_label << ',' << ALLOC_INFO[alloc].name << ',' << d2h_ms << ',';
    out << std::setprecision(3) << d2h_gbps;

    for (int i = 0; i < n_strategies; ++i) {
        const StratResult& r = results[i];
        out << ',' << std::setprecision(2) << r.mean_ms
            << ',' << std::setprecision(3) << r.bw
            << ',' << std::setprecision(2) << r.std_ms
            << ',' << std::setprecision(2) << compute_std_percent(r.mean_ms, r.std_ms)
            << ',' << std::setprecision(2) << r.median_ms
            << ',' << std::setprecision(2) << r.min_ms
            << ',' << std::setprecision(2) << r.max_ms
            << ',' << verify_status_str(r.ok);
    }
    return out.str();
}
