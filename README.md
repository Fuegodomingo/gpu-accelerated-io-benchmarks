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

```
```
