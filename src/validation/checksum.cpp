/** @file checksum.cpp @brief Checksum implementation. */

#include "validation/checksum.h"

double compute_sum_checksum(const double* buf, std::size_t n_doubles)
{
    double sum = 0.0;
    for (std::size_t i = 0; i < n_doubles; ++i) sum += buf[i];
    return sum;
}
