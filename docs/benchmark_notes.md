# Benchmark notes

## Executables

The benchmark suite has three entry points:

* `benchmarks/posix_main.cpp` — no MPI; multi-GPU POSIX write strategies.
* `benchmarks/mpi_main.cpp` — MPI-IO Independent / Collective / Direct I/O.
* `benchmarks/hdf5_main.cpp` — HDF5 Independent / Collective / Collective Chunked / Direct I/O.

## Header / source mapping

| Header                                | Implementation                                                    |
| ------------------------------------- | ----------------------------------------------------------------- |
| `include/benchmark/bench_params.h`    | `src/benchmark/bench_params.cpp`                                  |
| `include/benchmark/stats.h`           | `src/benchmark/stats.cpp`                                         |
| `include/benchmark/timer.h`           | `src/benchmark/timer.cpp`                                         |
| `include/benchmark/mpi_timer.h`       | `src/benchmark/mpi_timer.cpp`                                     |
| `include/io/mpi_file_write.h`         | `src/io/mpi_file_write.cpp`                                       |
| `include/io/hdf5_config.h`            | `src/io/hdf5_config.cpp`                                          |
| `include/io/hdf5_write.h`             | `src/io/hdf5_write.cpp`                                           |
| `include/io/posix_file_write.h`       | `src/io/posix_file_write.cpp`                                     |
| `include/output/csv.h`                | `src/output/csv.cpp`                                              |
| `include/output/report.h`             | `src/output/report.cpp`                                           |
| `include/transfer/d2h_transfer.h`     | `src/transfer/d2h_transfer.cpp`                                   |
| `include/validation/checksum.h`       | `src/validation/checksum.cpp`                                     |
| `include/validation/verify.h`         | `src/validation/verify_raw.cpp`, `src/validation/verify_hdf5.cpp` |
| `include/validation/runtime_checks.h` | header-only macros                                                |

`verify.h` deliberately has two implementation files so the POSIX executable can link raw-file verification without acquiring an HDF5/MPI dependency.

## Important benchmark behavior

* Aggregate write bandwidth is computed as `total bytes / mean wall time`.
* MPI writes larger than the legacy `int` count limit are split at the largest 4 MiB-aligned request below `INT_MAX`.
* Timed MPI writes include `MPI_File_sync`.
* Timed HDF5 writes include `H5Fflush`.
* HDF5 datasets are created collectively and timed datasets are pre-created outside the timed region.
* Lustre stripe count and stripe size are not overridden through `MPI_Info`. The layout configured externally with `lfs setstripe` is authoritative.

## Build/source groups

`src/transfer/d2h_transfer.cpp` contains HIP code and must be compiled using the same HIP-capable C++ compiler as the benchmark entry points.

### POSIX

The no-MPI POSIX executable links:

* `benchmarks/posix_main.cpp`
* `src/benchmark/bench_params.cpp`
* `src/benchmark/stats.cpp`
* `src/benchmark/timer.cpp`
* `src/io/posix_file_write.cpp`
* `src/output/csv.cpp`
* `src/output/report.cpp`
* `src/transfer/d2h_transfer.cpp`
* `src/validation/verify_raw.cpp`

### MPI

The MPI executable additionally uses:

* `benchmarks/mpi_main.cpp`
* `src/benchmark/mpi_timer.cpp`
* `src/io/mpi_file_write.cpp`

It also links the common benchmark, output, transfer, and raw verification sources listed above.

### HDF5

The HDF5 executable uses:

* `benchmarks/hdf5_main.cpp`
* `src/benchmark/mpi_timer.cpp`
* `src/io/hdf5_config.cpp`
* `src/io/hdf5_write.cpp`
* `src/validation/checksum.cpp`
* `src/validation/verify_raw.cpp`
* `src/validation/verify_hdf5.cpp`

It must be linked against the parallel HDF5 and MPI stack.

## Building on Adastra

The three benchmark executables can be built using the provided helper script:

```bash
./scripts/build.sh
```

The script loads the required Cray, MPI, parallel HDF5, and ROCm modules and builds the existing benchmark source tree without modifying it.

The resulting executables are written to:

```text
build/bin/bench_posix
build/bin/bench_mpi
build/bin/bench_hdf5
```

The build script may therefore be used independently of the smoke test when the full benchmark configuration should be compiled.

## Smoke test

A lightweight smoke test is provided to validate the complete benchmark workflow without launching the full benchmark configuration:

```bash
./scripts/smoke_test.sh
```

The script deliberately leaves the real benchmark source tree untouched.

Instead, it creates a temporary copy of:

```text
benchmarks/
include/
src/
```

under:

```text
.smoke_build/source/
```

Only this disposable copy is modified for the smoke test.

The default smoke-test configuration uses:

* 4 MiB per rank/GPU
* one timed iteration
* pinned host memory only
* output verification enabled
* one MI250X node in shared mode
* two GPU GCDs
* Lustre striping of 2 stripes × 1 MiB

All write strategies implemented by each enabled benchmark family are exercised.

The smoke test therefore checks the complete path:

```text
build
  ↓
Slurm submission
  ↓
GPU-to-host transfer
  ↓
POSIX / MPI / HDF5 writes
  ↓
output verification
  ↓
CSV generation
  ↓
plot generation
```

The test is intended for functional validation only. Its timings and bandwidth values should not be interpreted as representative benchmark performance because only one small data size and one timed iteration are used.

### Adastra project account

The script uses the active Adastra project selected for the current login session through:

```bash
ACTIVE_PROJECT
```

The generated Slurm job therefore charges the currently active project rather than hardcoding a user-specific account.

### Benchmark families

The following variables near the top of `smoke_test.sh` control which benchmark families are executed:

```bash
RUN_POSIX=1
RUN_MPI=1
RUN_HDF5=1
```

Set a value to `0` to skip the corresponding benchmark family.

For example:

```bash
RUN_POSIX=0
RUN_MPI=1
RUN_HDF5=1
```

runs only the MPI and HDF5 smoke tests.

Individual write strategies are not selected through the script because strategy selection is handled internally by the existing benchmark executables.

### Lustre striping

The Lustre layout used by the smoke test can be changed without modifying the benchmark source code.

The default configuration is:

```bash
STRIPE_COUNT=2
STRIPE_SIZE="1M"
```

Example alternatives include:

```bash
STRIPE_COUNT=1
STRIPE_SIZE="1M"
```

```bash
STRIPE_COUNT=4
STRIPE_SIZE="1M"
```

```bash
STRIPE_COUNT=8
STRIPE_SIZE="4M"
```

```bash
STRIPE_COUNT=16
STRIPE_SIZE="4M"
```

To keep the inherited/default filesystem layout:

```bash
STRIPE_COUNT=""
STRIPE_SIZE=""
```

The script applies the selected layout to the result directory before benchmark files are created, allowing the files to inherit the requested Lustre configuration.

### Smoke-test results

Results are written to:

```text
results/smoke_<jobid>/
```

A completed run may contain:

```text
posix.log
posix_bench.csv

mpi.log
mpi_bench.csv

hdf5.log
hdf5_bench.csv

plots/
```

Slurm stdout and stderr are written under:

```text
.smoke_build/smoke-<jobid>.out
.smoke_build/smoke-<jobid>.err
```

A successful smoke test should show `OK` verification results for every executed write strategy.

For MPI Direct I/O, the output should also report:

```text
direct_io hint reported by ROMIO: true
```

## Plotting benchmark results

The `plot_benchmark.py` script generates bandwidth plots directly from the CSV files produced by the POSIX, MPI, and HDF5 benchmarks.

It automatically detects the available allocation types and write strategies, so benchmark values do not need to be hardcoded into the plotting script.

Basic usage is:

```bash
python plots/plot_benchmark.py <benchmark.csv>
```

For example:

```bash
python plots/plot_benchmark.py results/smoke_1234567/mpi_bench.csv
```

By default, the script generates:

* a D2H bandwidth comparison by allocation type
* one write-bandwidth plot per allocation type

Additional strategy-oriented plots can be enabled with:

```bash
--strategy-plots
```

Optional theoretical bandwidth reference lines can be added using:

```bash
--d2h-reference
```

and:

```bash
--write-reference
```

Output figures are saved automatically under:

```text
plots/<csv_name>/
```

When launched through `smoke_test.sh`, the Adastra Python module is loaded automatically and the plotting script is executed after the benchmark CSV files have been generated.

Plot generation is optional and controlled by:

```bash
PLOT_RESULTS=1
```

Set:

```bash
PLOT_RESULTS=0
```

to disable automatic plotting.
