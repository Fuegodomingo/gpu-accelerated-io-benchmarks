#pragma once

/** @file checksum.h @brief Lightweight checksums used by validation helpers. */

#include <cstddef>

/** @brief Sum all elements of a double buffer. */
double compute_sum_checksum(const double* buf, std::size_t n_doubles);
