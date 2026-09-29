/** @file posix_file_write.cpp @brief POSIX multi-GPU write implementations. */

#include "io/posix_file_write.h"

#include "benchmark/bench_params.h"
#include "benchmark/stats.h"
#include "benchmark/timer.h"
#include "validation/verify.h"

#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

namespace {

constexpr std::size_t MERGE_BUFFER_BYTES = 64ULL << 20;

[[noreturn]] void fail_errno(const char* operation)
{
    std::fprintf(stderr, "%s failed: %s\n", operation, std::strerror(errno));
    std::abort();
}

void write_all_fd(int fd, const void* data, std::size_t bytes)
{
    const char* src = static_cast<const char*>(data);
    std::size_t remaining = bytes;
    while (remaining > 0) {
        const ssize_t n = ::write(fd, src, remaining);
        if (n < 0) {
            if (errno == EINTR) continue;
            fail_errno("write");
        }
        if (n == 0) { std::fprintf(stderr, "write made no progress\n"); std::abort(); }
        src += n;
        remaining -= static_cast<std::size_t>(n);
    }
}

void pwrite_all(int fd, const void* data, std::size_t bytes, off_t offset)
{
    const char* src = static_cast<const char*>(data);
    std::size_t remaining = bytes;
    while (remaining > 0) {
        const ssize_t n = ::pwrite(fd, src, remaining, offset);
        if (n < 0) {
            if (errno == EINTR) continue;
            fail_errno("pwrite");
        }
        if (n == 0) { std::fprintf(stderr, "pwrite made no progress\n"); std::abort(); }
        src += n;
        remaining -= static_cast<std::size_t>(n);
        offset += n;
    }
}

double run_individual_merge_once(float* const* buffers, int device_count,
                                 std::size_t size, const std::string& merged_path)
{
    const double t0 = host_time_sec();
    std::vector<std::thread> workers;
    workers.reserve(static_cast<std::size_t>(device_count));

    for (int d = 0; d < device_count; ++d) {
        workers.emplace_back([=]() {
            const std::string path = "posix_tmp_gpu" + std::to_string(d) + ".bin";
            const int fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) fail_errno("open temporary file");
            write_all_fd(fd, buffers[d], size);
            if (::fsync(fd) != 0) fail_errno("fsync temporary file");
            ::close(fd);
        });
    }
    for (auto& worker : workers) worker.join();

    const int out = ::open(merged_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (out < 0) fail_errno("open merged file");
    std::vector<char> scratch(std::min(size, MERGE_BUFFER_BYTES));

    for (int d = 0; d < device_count; ++d) {
        const std::string path = "posix_tmp_gpu" + std::to_string(d) + ".bin";
        const int in = ::open(path.c_str(), O_RDONLY);
        if (in < 0) fail_errno("open temporary file for merge");

        std::size_t remaining = size;
        while (remaining > 0) {
            const std::size_t request = std::min(remaining, scratch.size());
            const ssize_t n = ::read(in, scratch.data(), request);
            if (n < 0) {
                if (errno == EINTR) continue;
                fail_errno("read temporary file");
            }
            if (n == 0) { std::fprintf(stderr, "unexpected EOF during merge\n"); std::abort(); }
            write_all_fd(out, scratch.data(), static_cast<std::size_t>(n));
            remaining -= static_cast<std::size_t>(n);
        }
        ::close(in);
        ::remove(path.c_str());
    }

    if (::fsync(out) != 0) fail_errno("fsync merged file");
    ::close(out);
    return host_elapsed_ms(t0, host_time_sec());
}

double run_pwrite_once(float* const* buffers, int device_count, std::size_t size,
                       int fd)
{
    const double t0 = host_time_sec();
    std::vector<std::thread> workers;
    workers.reserve(static_cast<std::size_t>(device_count));
    for (int d = 0; d < device_count; ++d) {
        workers.emplace_back([=]() {
            pwrite_all(fd, buffers[d], size,
                       static_cast<off_t>(d) * static_cast<off_t>(size));
        });
    }
    for (auto& worker : workers) worker.join();
    if (::fsync(fd) != 0) fail_errno("fsync pwrite file");
    return host_elapsed_ms(t0, host_time_sec());
}

double run_mutex_once(float* const* buffers, int device_count, std::size_t size,
                      FILE* file)
{
    std::rewind(file);
    std::mutex write_mutex;
    const double t0 = host_time_sec();

    std::vector<std::thread> workers;
    workers.reserve(static_cast<std::size_t>(device_count));
    for (int d = 0; d < device_count; ++d) {
        workers.emplace_back([&, d]() {
            std::lock_guard<std::mutex> lock(write_mutex);
            if (std::fwrite(buffers[d], 1, size, file) != size) {
                std::fprintf(stderr, "fwrite failed in mutex strategy\n");
                std::abort();
            }
        });
    }
    for (auto& worker : workers) worker.join();

    if (std::fflush(file) != 0) fail_errno("fflush mutex file");
    if (::fsync(fileno(file)) != 0) fail_errno("fsync mutex file");
    return host_elapsed_ms(t0, host_time_sec());
}

} // namespace

StratResult benchmark_posix_strategy(float* const* buffers, int device_count,
                                     std::size_t size, AllocType alloc,
                                     PosixWriteStrategy strat)
{
    const std::string path = posix_strat_file(strat, alloc);
    double runs[NRUNS]{};

    int fd = -1;
    FILE* file = nullptr;
    if (strat == POSIX_STRAT_PWRITE) {
        fd = ::open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) fail_errno("open pwrite file");
        if (::ftruncate(fd, static_cast<off_t>(device_count) * static_cast<off_t>(size)) != 0)
            fail_errno("ftruncate pwrite file");
    } else if (strat == POSIX_STRAT_MUTEX) {
        file = std::fopen(path.c_str(), "wb+");
        if (!file) fail_errno("fopen mutex file");
    }

    auto run_once = [&]() -> double {
        switch (strat) {
            case POSIX_STRAT_INDIVIDUAL_MERGE:
                return run_individual_merge_once(buffers, device_count, size, path);
            case POSIX_STRAT_PWRITE:
                return run_pwrite_once(buffers, device_count, size, fd);
            case POSIX_STRAT_MUTEX:
                return run_mutex_once(buffers, device_count, size, file);
            default:
                std::abort();
        }
    };

    run_once(); // warmup
    for (int r = 0; r < NRUNS; ++r) runs[r] = run_once();

    if (fd >= 0) ::close(fd);
    if (file) std::fclose(file);

    const int verification = strat == POSIX_STRAT_MUTEX
        ? verify_raw_file_unordered_blocks(path.c_str(), device_count, size)
        : verify_raw_file(path.c_str(), device_count, size);

    StratResult result = summarize_runs(runs, NRUNS,
        static_cast<std::size_t>(device_count) * size, verification);
    ::remove(path.c_str());
    return result;
}
