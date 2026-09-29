#!/usr/bin/env bash
set -euo pipefail

# Build the existing MultiGPU sources without modifying them.
#
# Optional overrides:
#   SOURCE_ROOT=/path/to/source  BUILD_DIR=/path/to/build  ./scripts/build.sh
#
# On Adastra this expects the standard Cray/ROCm modules below.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DEFAULT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
SOURCE_ROOT="${SOURCE_ROOT:-${DEFAULT_ROOT}}"
BUILD_DIR="${BUILD_DIR:-${SOURCE_ROOT}/build}"
BIN_DIR="${BUILD_DIR}/bin"

module purge
module load cpe/24.07
module load craype-accel-amd-gfx90a
module load craype-x86-trento
module load PrgEnv-cray
module load cray-mpich
module load cray-hdf5-parallel
module load rocm/6.3.3

mkdir -p "${BIN_DIR}"

CXX="${CXX:-CC}"
COMMON_FLAGS=(
    --rocm-path="${ROCM_PATH}"
    -x hip
    -std=c++17
    -O3
    -I"${SOURCE_ROOT}/include"
)

COMMON_SOURCES=(
    "${SOURCE_ROOT}/src/benchmark/bench_params.cpp"
    "${SOURCE_ROOT}/src/benchmark/stats.cpp"
    "${SOURCE_ROOT}/src/benchmark/timer.cpp"
    "${SOURCE_ROOT}/src/output/csv.cpp"
    "${SOURCE_ROOT}/src/output/report.cpp"
    "${SOURCE_ROOT}/src/transfer/d2h_transfer.cpp"
)

echo "==> Building POSIX benchmark"
"${CXX}" "${COMMON_FLAGS[@]}" -pthread \
    "${SOURCE_ROOT}/benchmarks/posix_main.cpp" \
    "${COMMON_SOURCES[@]}" \
    "${SOURCE_ROOT}/src/io/posix_file_write.cpp" \
    "${SOURCE_ROOT}/src/validation/verify_raw.cpp" \
    -lamdhip64 \
    -o "${BIN_DIR}/bench_posix"

echo "==> Building MPI benchmark"
"${CXX}" "${COMMON_FLAGS[@]}" \
    "${SOURCE_ROOT}/benchmarks/mpi_main.cpp" \
    "${COMMON_SOURCES[@]}" \
    "${SOURCE_ROOT}/src/benchmark/mpi_timer.cpp" \
    "${SOURCE_ROOT}/src/io/mpi_file_write.cpp" \
    "${SOURCE_ROOT}/src/validation/verify_raw.cpp" \
    -lamdhip64 \
    -o "${BIN_DIR}/bench_mpi"

echo "==> Building HDF5 benchmark"
"${CXX}" "${COMMON_FLAGS[@]}" \
    "${SOURCE_ROOT}/benchmarks/hdf5_main.cpp" \
    "${COMMON_SOURCES[@]}" \
    "${SOURCE_ROOT}/src/benchmark/mpi_timer.cpp" \
    "${SOURCE_ROOT}/src/io/hdf5_config.cpp" \
    "${SOURCE_ROOT}/src/io/hdf5_write.cpp" \
    "${SOURCE_ROOT}/src/validation/checksum.cpp" \
    "${SOURCE_ROOT}/src/validation/verify_raw.cpp" \
    "${SOURCE_ROOT}/src/validation/verify_hdf5.cpp" \
    -lamdhip64 \
    -o "${BIN_DIR}/bench_hdf5"

echo
echo "Build complete:"
ls -lh \
    "${BIN_DIR}/bench_posix" \
    "${BIN_DIR}/bench_mpi" \
    "${BIN_DIR}/bench_hdf5"
