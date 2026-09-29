/** @file verify_raw.cpp @brief Streaming validation for raw benchmark files. */

#include "validation/verify.h"

#include <algorithm>
#include <cstdio>
#include <vector>

namespace {

constexpr std::size_t VERIFY_CHUNK_BYTES = 64ULL << 20;

bool verify_float_chunk(const float* data, std::size_t count, float expected)
{
    for (std::size_t i = 0; i < count; ++i) {
        if (data[i] != expected) return false;
    }
    return true;
}

} // namespace

const char* verify_status_str(int status)
{
    if (status == VERIFY_OK) return "OK";
    if (status == VERIFY_FAIL) return "FAIL";
    return "?";
}

int verify_raw_file(const char* path, int participants, std::size_t size_per_participant)
{
    if (size_per_participant % sizeof(float) != 0) return VERIFY_ERROR;
    FILE* file = std::fopen(path, "rb");
    if (!file) return VERIFY_ERROR;

    const std::size_t floats_per_chunk = VERIFY_CHUNK_BYTES / sizeof(float);
    std::vector<float> buffer(std::min(size_per_participant / sizeof(float),
                                       floats_per_chunk));

    for (int p = 0; p < participants; ++p) {
        std::size_t remaining = size_per_participant / sizeof(float);
        while (remaining > 0) {
            const std::size_t count = std::min(remaining, floats_per_chunk);
            if (std::fread(buffer.data(), sizeof(float), count, file) != count) {
                std::fclose(file);
                return VERIFY_ERROR;
            }
            if (!verify_float_chunk(buffer.data(), count, static_cast<float>(p))) {
                std::fclose(file);
                return VERIFY_FAIL;
            }
            remaining -= count;
        }
    }

    std::fclose(file);
    return VERIFY_OK;
}

int verify_raw_file_unordered_blocks(const char* path, int participants,
                                     std::size_t size_per_participant)
{
    if (size_per_participant % sizeof(float) != 0) return VERIFY_ERROR;
    FILE* file = std::fopen(path, "rb");
    if (!file) return VERIFY_ERROR;

    const std::size_t floats_per_chunk = VERIFY_CHUNK_BYTES / sizeof(float);
    std::vector<float> buffer(std::min(size_per_participant / sizeof(float),
                                       floats_per_chunk));
    std::vector<bool> seen(static_cast<std::size_t>(participants), false);

    for (int block = 0; block < participants; ++block) {
        std::size_t remaining = size_per_participant / sizeof(float);
        int expected_rank = -1;
        while (remaining > 0) {
            const std::size_t count = std::min(remaining, floats_per_chunk);
            if (std::fread(buffer.data(), sizeof(float), count, file) != count) {
                std::fclose(file);
                return VERIFY_ERROR;
            }
            if (expected_rank < 0) {
                expected_rank = static_cast<int>(buffer[0]);
                if (expected_rank < 0 || expected_rank >= participants ||
                    seen[static_cast<std::size_t>(expected_rank)]) {
                    std::fclose(file);
                    return VERIFY_FAIL;
                }
            }
            if (!verify_float_chunk(buffer.data(), count,
                                    static_cast<float>(expected_rank))) {
                std::fclose(file);
                return VERIFY_FAIL;
            }
            remaining -= count;
        }
        seen[static_cast<std::size_t>(expected_rank)] = true;
    }

    std::fclose(file);
    return VERIFY_OK;
}
