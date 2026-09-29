# GPU I/O Benchmarks

HPC benchmarks for GPU-to-storage I/O using MPI, HDF5, CUDA, and HIP.

This repository contains a benchmark suite developed during a research internship at **CEA / Maison de la Simulation** to investigate data movement between GPUs and parallel storage systems on HPC platforms.

The suite compares several I/O approaches and configurations, including POSIX I/O, MPI-IO, parallel HDF5, Direct I/O, pinned host memory, and Lustre striping.

---

## Overview

GPU-accelerated applications can generate large amounts of data that must eventually be transferred from GPU memory to storage. The performance of this path depends on several components:

```text
GPU memory
    │
    │ D2H transfer
    ▼
Host memory
    │
    │ POSIX / MPI-IO / HDF5
    ▼
Parallel filesystem
    │
    │ Lustre striping
    ▼
Storage
```

This project provides reproducible benchmarks for studying the different stages of this pipeline and their interaction.

The benchmarks were developed and tested on HPC systems including **Ruche** and **Adastra**, with the final benchmark suite targeting GPU-equipped Adastra nodes.

---

## Features

The benchmark suite provides:

* Multi-GPU POSIX write benchmarks
* MPI-IO Independent and Collective writes
* MPI-IO Direct I/O
* Parallel HDF5 Independent and Collective writes
* HDF5 Collective Chunked writes
* HDF5 Direct I/O
* GPU-to-host transfers using HIP
* Pinned host memory
* Output verification and checksums
* CSV benchmark output
* Automatic report generation
* Benchmark plotting
* Configurable Lustre striping
* A lightweight end-to-end smoke test

---

## Selected Results

The benchmarks show that I/O performance depends strongly on the interaction between the I/O strategy, memory configuration, and Lustre filesystem layout.

### MPI-IO

For a 16 GB/rank workload using pinned host memory, changing the Lustre configuration had a particularly strong effect on collective MPI-IO performance:

| Lustre layout       |   Independent |    Collective |
| ------------------- | ------------: | ------------: |
| 2 × 1 MiB (default) |     2.28 GB/s |     1.05 GB/s |
| 8 × 1 MiB           |     2.21 GB/s |     1.06 GB/s |
| **8 × 4 MiB**       | **2.22 GB/s** | **2.09 GB/s** |
| 16 × 1 MiB          |     2.25 GB/s |     0.93 GB/s |
| 16 × 4 MiB          |     2.24 GB/s |     1.90 GB/s |

This illustrates that increasing the stripe count alone does not necessarily improve performance: the stripe size and I/O access pattern also matter.

### Direct I/O

The Direct I/O experiments showed a substantial difference between ordinary and pinned host memory. For a 1 GB/rank HDF5 Direct I/O workload on a `2 × 1 MiB` layout, pinned memory achieved **6.94 GB/s**, compared with **2.74 GB/s** for pageable memory.

At larger transfer sizes, filesystem configuration became increasingly important. For example, with a `24 × 2 MiB` layout and 1 GB/rank, HDF5 Direct I/O reached approximately **52–53 GB/s** with pinned or managed memory.

### Application-level validation with Smilei

The benchmark results were subsequently evaluated using the Smilei particle-in-cell simulation framework.

A 2D Smilei experiment showed a great impact on I/O when comparing a single Lustre stripe with the default and wider layouts: diagnostic time decreased from **863.1 s (`1 × 1 MiB`) to 268.8 s (`8 × 1 MiB`)**, while the fraction of runtime spent in diagnostics decreased from **41.3% to 18.0%**.

In an 8-GPU 3D simulation, changing the Lustre layout from the default `2 × 1 MiB` to `4 × 1 MiB` reduced the `Fields0` diagnostic time from **460 s to 420 s** and total wall time from **963 s to 921 s**. This corresponds to approximately:

* **8.7% less time spent in the field diagnostic**
* **4.4% lower total wall time**
* Effective write rate increasing from approximately **1.01 GB/s to 1.11 GB/s**

These experiments highlight an important aspect of HPC I/O optimization: **a configuration that performs well in a synthetic benchmark must ultimately be evaluated in the context of the application workload it is intended to accelerate.**


## Repository Structure

```text
.
├── benchmarks/
│   ├── posix_main.cpp       # Multi-GPU POSIX benchmarks
│   ├── mpi_main.cpp         # MPI-IO benchmarks
│   └── hdf5_main.cpp        # HDF5 benchmarks
│
├── include/
│   ├── benchmark/           # Parameters, timers and statistics
│   ├── io/                  # POSIX, MPI-IO and HDF5 implementations
│   ├── output/              # CSV and report generation
│   ├── transfer/            # GPU-to-host transfer
│   └── validation/          # Output verification and runtime checks
│
├── src/
│   ├── benchmark/
│   ├── io/
│   ├── output/
│   ├── transfer/
│   └── validation/
│
├── plots/
│   └── plot_benchmark.py    # Benchmark plotting utility
│
├── scripts/
│   ├── build.sh             # Adastra build script
│   └── smoke_test.sh        # End-to-end smoke test
│
└── README.md
```

More detailed implementation notes and source-to-header mappings are provided separately in [`Benchmark notes`](Benchmark%20notes).

---

## Benchmark Executables

The suite has three main entry points.

### POSIX

```text
benchmarks/posix_main.cpp
```

Provides multi-GPU POSIX write strategies without MPI.

### MPI-IO

```text
benchmarks/mpi_main.cpp
```

Provides:

* MPI-IO Independent
* MPI-IO Collective
* MPI-IO Direct I/O

### HDF5

```text
benchmarks/hdf5_main.cpp
```

Provides:

* HDF5 Independent
* HDF5 Collective
* HDF5 Collective Chunked
* HDF5 Direct I/O

All three executables share the common benchmark, transfer, output, and validation infrastructure where appropriate.

---

## Building on Adastra

The provided build script loads the required HPC environment and builds all three executables:

```bash
./scripts/build.sh
```

The resulting binaries are:

```text
build/bin/bench_posix
build/bin/bench_mpi
build/bin/bench_hdf5
```

The build environment uses the Cray programming environment together with MPI, parallel HDF5, and ROCm.

The GPU transfer implementation in:

```text
src/transfer/d2h_transfer.cpp
```

contains HIP code and must therefore be compiled with a HIP-capable C++ compiler.

---

## Smoke Test

A lightweight end-to-end smoke test is provided to verify that the complete benchmark workflow is functional:

```bash
./scripts/smoke_test.sh
```

The smoke test is intentionally small and is designed for **functional validation rather than performance measurement**.

It exercises the complete pipeline:

```text
Build
  ↓
Slurm submission
  ↓
GPU → host transfer
  ↓
POSIX / MPI-IO / HDF5
  ↓
Output verification
  ↓
CSV generation
  ↓
Plot generation
```

### Default configuration

The default smoke test uses:

* 4 MiB per rank/GPU
* one timed iteration
* pinned host memory
* output verification
* one MI250X node
* two GPU GCDs
* Lustre striping: 2 × 1 MiB

All write strategies implemented by the enabled benchmark families are exercised.

Because the dataset is intentionally small and only one timed iteration is performed, the resulting bandwidth values **should not be interpreted as representative performance measurements**.

---

## Selecting Benchmark Families

The smoke test can independently enable or disable the three benchmark families:

```bash
RUN_POSIX=1
RUN_MPI=1
RUN_HDF5=1
```

For example:

```bash
RUN_POSIX=0
RUN_MPI=1
RUN_HDF5=1
```

runs only the MPI-IO and HDF5 benchmarks.

Individual write strategies are selected internally by the benchmark executables.

---

## Lustre Striping

Lustre striping can be configured directly from the smoke-test script without modifying the benchmark source code.

The default configuration is:

```bash
STRIPE_COUNT=2
STRIPE_SIZE="1M"
```

For example:

```bash
STRIPE_COUNT=8
STRIPE_SIZE="4M"
```

or:

```bash
STRIPE_COUNT=16
STRIPE_SIZE="4M"
```

To use the inherited/default filesystem layout:

```bash
STRIPE_COUNT=""
STRIPE_SIZE=""
```

The selected layout is applied to the result directory before benchmark files are created, allowing the files to inherit the requested Lustre configuration.

This makes it possible to compare different filesystem layouts while keeping the benchmark code unchanged.

---

## Benchmark Results

Benchmark output is written to CSV files, which can then be visualized using the included plotting utility.

A typical result directory contains:

```text
results/
└── smoke_<jobid>/
    ├── posix.log
    ├── posix_bench.csv
    ├── mpi.log
    ├── mpi_bench.csv
    ├── hdf5.log
    ├── hdf5_bench.csv
    └── plots/
```

Slurm output is stored separately under:

```text
.smoke_build/
```

---

## Plotting

The included plotting script reads the benchmark CSV files directly:

```bash
python plots/plot_benchmark.py <benchmark.csv>
```

For example:

```bash
python plots/plot_benchmark.py \
    results/smoke_1234567/mpi_bench.csv
```

The script automatically detects the allocation types and write strategies present in the CSV file.

It generates:

* D2H bandwidth comparisons by allocation type
* Write-bandwidth plots for each allocation type

Additional strategy-oriented plots can be enabled with:

```bash
python plots/plot_benchmark.py <benchmark.csv> --strategy-plots
```

Optional theoretical reference lines can be added with:

```bash
--d2h-reference
```

and:

```bash
--write-reference
```

Figures are written to:

```text
plots/<csv_name>/
```

Automatic plotting from the smoke test can be disabled with:

```bash
PLOT_RESULTS=0
```

---

## Measurement Details

The benchmark suite follows several conventions to make measurements consistent.

### Bandwidth

Aggregate write bandwidth is computed as:

```text
total bytes / mean wall time
```

### MPI-IO

Timed MPI writes include:

```cpp
MPI_File_sync
```

Large MPI writes exceeding the legacy `int` count limit are split into requests aligned to 4 MiB boundaries.

### HDF5

Timed HDF5 writes include:

```cpp
H5Fflush
```

Datasets are created collectively and pre-created outside the timed region so that dataset creation is not included in the measured write time.

### Lustre

Lustre stripe count and stripe size are configured externally using the filesystem layout.

The benchmark does **not** override the layout through `MPI_Info`; the layout configured with `lfs setstripe` is authoritative.

---

## Validation

The benchmark suite includes output verification to ensure that performance measurements are not obtained at the expense of incorrect data.

The validation infrastructure supports both:

* raw-file verification
* HDF5 verification

The HDF5 and raw-file verification implementations are intentionally separated so that the POSIX benchmark can perform raw-file verification without introducing an unnecessary HDF5 dependency.

---

## Reproducibility

When comparing benchmark configurations, the following parameters should be recorded:

* HPC system
* GPU model
* number of GPUs / MPI ranks
* data size per rank
* host-memory allocation type
* I/O interface
* MPI-IO access mode
* Direct I/O configuration
* Lustre stripe count
* Lustre stripe size
* number of timed iterations

Filesystem load and system configuration can affect measured bandwidth, so results should be interpreted in the context of the machine and filesystem configuration used.

---

## From Synthetic Benchmarks to Scientific Workloads

The benchmark suite was developed as part of a broader investigation into GPU-to-storage performance for scientific simulations.

After studying the individual components of the I/O path, the resulting observations were evaluated using **Smilei**, a particle-in-cell simulation framework.

This application-level validation helped determine whether the trends observed in synthetic benchmarks translated to a realistic scientific workload.

---

## Project Context

This work was carried out during a research internship at **CEA / Maison de la Simulation**.

The project focused on **GPU-to-storage I/O performance in HPC systems**, with experiments conducted on the Ruche and Adastra platforms.

The work involved GPU programming, parallel I/O, filesystem configuration, benchmarking methodology, and performance analysis.

---

## License

This project is released under the license specified in [`LICENSE`](LICENSE).
