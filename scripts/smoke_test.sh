#!/usr/bin/env bash
set -euo pipefail

# ============================================================================
# MultiGPU smoke test
#
# IMPORTANT:
#   This script NEVER edits the real benchmark source tree.
#   It copies benchmarks/include/src to .smoke_build/source and changes only
#   that disposable copy to:
#       - 4 MiB per rank/GPU
#       - 1 timed run
#       - pinned host memory only
#       - verification enabled
#
# The real project sources remain untouched.
# ============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

# ------------------------- USER CONFIGURATION -------------------------------

# Use the active Adastra project selected for this login session.
ACCOUNT="${ACTIVE_PROJECT:-}"

if [[ -z "${ACCOUNT}" ]]; then
    echo "ERROR: ACTIVE_PROJECT is not set."
    echo "Activate a project with 'myproject -a <project>' before running this script."
    exit 1
fi

echo "Using Adastra project account: ${ACCOUNT}"

# Run whole benchmark families. Individual write strategies are intentionally
RUN_POSIX=1
RUN_MPI=1
RUN_HDF5=1

# Lustre layout applied to the result directory before files are created.
# Use empty values to keep the filesystem/default inherited layout.
STRIPE_COUNT=2
STRIPE_SIZE="1M"

# Alternative examples:
# STRIPE_COUNT=1;  STRIPE_SIZE="1M"
# STRIPE_COUNT=4;  STRIPE_SIZE="1M"
# STRIPE_COUNT=8;  STRIPE_SIZE="4M"
# STRIPE_COUNT=16; STRIPE_SIZE="4M"
# STRIPE_COUNT=""; STRIPE_SIZE=""

PLOT_RESULTS=1

# Small shared-node request.
NODES=1
TASKS_PER_NODE=2
GPUS_PER_NODE=2
CPUS_PER_TASK=8
TIME_LIMIT="00:05:00"

# ---------------------------------------------------------------------------

SMOKE_DIR="${ROOT}/.smoke_build"
SMOKE_SOURCE="${SMOKE_DIR}/source"
SMOKE_BUILD="${SMOKE_DIR}/build"

echo "==> Preparing disposable smoke-test source copy"
rm -rf "${SMOKE_DIR}"
mkdir -p "${SMOKE_SOURCE}"

cp -a "${ROOT}/benchmarks" "${SMOKE_SOURCE}/"
cp -a "${ROOT}/include" "${SMOKE_SOURCE}/"
cp -a "${ROOT}/src" "${SMOKE_SOURCE}/"

# Change only the temporary copy.
python3 - "${SMOKE_SOURCE}/include/benchmark/bench_params.h" <<'PY'
from pathlib import Path
import re
import sys

path = Path(sys.argv[1])
text = path.read_text()

text, n = re.subn(
    r"inline constexpr int NRUNS\s*=\s*\d+\s*;",
    "inline constexpr int NRUNS = 1;",
    text,
    count=1,
)
assert n == 1, "Could not find NRUNS in temporary bench_params.h"

text, n = re.subn(
    r"inline constexpr std::size_t BENCHMARK_SIZES\[\]\s*=\s*\{.*?\};",
    "inline constexpr std::size_t BENCHMARK_SIZES[] = {\n"
    "    4ULL << 20\n"
    "};",
    text,
    count=1,
    flags=re.S,
)
assert n == 1, "Could not find BENCHMARK_SIZES in temporary bench_params.h"

text, n = re.subn(
    r"inline constexpr bool VERIFY_OUTPUT\s*=\s*(?:true|false)\s*;",
    "inline constexpr bool VERIFY_OUTPUT = true;",
    text,
    count=1,
)
assert n == 1, "Could not find VERIFY_OUTPUT in temporary bench_params.h"

text, n = re.subn(
    r"inline constexpr AllocType BENCHMARK_ALLOCS\[\]\s*=\s*\{.*?\};",
    "inline constexpr AllocType BENCHMARK_ALLOCS[] = {\n"
    "    ALLOC_PINNED\n"
    "};",
    text,
    count=1,
    flags=re.S,
)
assert n == 1, "Could not find BENCHMARK_ALLOCS in temporary bench_params.h"

path.write_text(text)
PY

echo "==> Building smoke executables from disposable copy"
SOURCE_ROOT="${SMOKE_SOURCE}" BUILD_DIR="${SMOKE_BUILD}" "${ROOT}/scripts/build.sh"

SBATCH_FILE="${SMOKE_DIR}/smoke.slurm"

cat > "${SBATCH_FILE}" <<EOF
#!/usr/bin/env bash
#SBATCH --job-name=multigpu_smoke
#SBATCH --constraint=MI250
#SBATCH --nodes=${NODES}
#SBATCH --ntasks-per-node=${TASKS_PER_NODE}
#SBATCH --gpus-per-node=${GPUS_PER_NODE}
#SBATCH --cpus-per-task=${CPUS_PER_TASK}
#SBATCH --threads-per-core=1
#SBATCH --time=${TIME_LIMIT}
#SBATCH --output=${SMOKE_DIR}/smoke-%j.out
#SBATCH --error=${SMOKE_DIR}/smoke-%j.err
EOF

echo "#SBATCH --account=${ACCOUNT}" >> "${SBATCH_FILE}"

cat >> "${SBATCH_FILE}" <<EOF

set -euo pipefail

module purge
module load cpe/24.07
module load craype-accel-amd-gfx90a
module load craype-x86-trento
module load PrgEnv-cray
module load cray-mpich
module load cray-hdf5-parallel
module load rocm/6.3.3
module load python

export MPICH_GPU_SUPPORT_ENABLED=1

ROOT="${ROOT}"
BIN_DIR="${SMOKE_BUILD}/bin"
RUN_POSIX="${RUN_POSIX}"
RUN_MPI="${RUN_MPI}"
RUN_HDF5="${RUN_HDF5}"
PLOT_RESULTS="${PLOT_RESULTS}"
STRIPE_COUNT="${STRIPE_COUNT}"
STRIPE_SIZE="${STRIPE_SIZE}"

# Check that plotting dependencies are available.
if [[ "\${PLOT_RESULTS}" == "1" ]]; then
    if ! python -c "import matplotlib" >/dev/null 2>&1; then
        echo "WARNING: matplotlib is unavailable; skipping plots." >&2
        PLOT_RESULTS=0
    fi
fi

RESULT_DIR="\${ROOT}/results/smoke_\${SLURM_JOB_ID}"
mkdir -p "\${RESULT_DIR}"

if [[ -n "\${STRIPE_COUNT}" && -n "\${STRIPE_SIZE}" ]]; then
    echo "Applying Lustre layout: count=\${STRIPE_COUNT}, size=\${STRIPE_SIZE}"
    lfs setstripe -c "\${STRIPE_COUNT}" -S "\${STRIPE_SIZE}" "\${RESULT_DIR}"
else
    echo "Using inherited/default Lustre layout"
fi

cd "\${RESULT_DIR}"

echo "Smoke-test parameters: 4 MiB, 1 run, pinned only, verification ON"
echo "Results: \${RESULT_DIR}"
echo

if [[ "\${RUN_POSIX}" == "1" ]]; then
    echo "================ POSIX ================"
    srun --ntasks=1 \
         --cpus-per-task=${CPUS_PER_TASK} \
         --gpus-per-task=${GPUS_PER_NODE} \
         --kill-on-bad-exit=1 \
         "\${BIN_DIR}/bench_posix" 2>&1 | tee posix.log
fi

if [[ "\${RUN_MPI}" == "1" ]]; then
    echo "================ MPI =================="
    srun --ntasks=${TASKS_PER_NODE} \
         --ntasks-per-node=${TASKS_PER_NODE} \
         --cpus-per-task=${CPUS_PER_TASK} \
         --gpus-per-task=1 \
         --gpu-bind=closest \
         --kill-on-bad-exit=1 \
         --label \
         "\${BIN_DIR}/bench_mpi" 2>&1 | tee mpi.log
fi

if [[ "\${RUN_HDF5}" == "1" ]]; then
    echo "================ HDF5 ================="
    srun --ntasks=${TASKS_PER_NODE} \
         --ntasks-per-node=${TASKS_PER_NODE} \
         --cpus-per-task=${CPUS_PER_TASK} \
         --gpus-per-task=1 \
         --gpu-bind=closest \
         --kill-on-bad-exit=1 \
         --label \
         "\${BIN_DIR}/bench_hdf5" 2>&1 | tee hdf5.log
fi

echo
echo "================ PLOTS ================"

if [[ "\${PLOT_RESULTS}" == "1" ]]; then
    for csv in posix_bench.csv mpi_bench.csv hdf5_bench.csv; do
        if [[ -f "\${csv}" ]]; then
            echo "Plotting \${csv}"
            if ! python "\${ROOT}/plots/plot_benchmark.py" "\${csv}"; then
                echo "WARNING: plotting failed for \${csv}; benchmark result is still kept." >&2
            fi
        fi
    done
fi

echo
echo "Smoke test finished."
echo "Results: \${RESULT_DIR}"
EOF

echo
echo "==> Submitting smoke test"
job_output="$(sbatch "${SBATCH_FILE}")"
echo "${job_output}"
echo
echo "The real source tree was not modified."
echo "Logs will appear under: ${SMOKE_DIR}/smoke-<jobid>.{out,err}"
echo "Benchmark results will appear under: ${ROOT}/results/smoke_<jobid>/"
