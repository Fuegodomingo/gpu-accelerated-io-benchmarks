# gpu-accelerated-io-benchmarks
HPC benchmarks for GPU-to-storage I/O using MPI, HDF5, CUDA, and HIP.

This repository contains the benchmarking code developed during my research internship at **CEA / Maison de la Simulation**, investigating data movement between GPUs and parallel storage systems on HPC platforms.

The project studies how GPU memory transfers, MPI-IO, HDF5, direct I/O, memory alignment, and Lustre striping affect I/O performance, with a final validation using the **Smilei** particle-in-cell simulation code.

---

## Overview

Modern HPC applications increasingly generate large amounts of data on accelerators, making efficient data movement between GPU memory and storage an important performance consideration.

The objective of this project was to investigate the performance of different I/O paths and configurations, with particular attention to:

* GPU-to-host memory transfers
* Host memory allocation and pinned memory
* POSIX and MPI-IO
* HDF5 parallel I/O
* Direct I/O
* Memory alignment requirements
* Lustre striping configuration
* Scaling with data size and number of MPI processes
* The impact of I/O configuration on a real HPC application

The experiments were initially developed and tested on **Ruche** and were subsequently adapted to **Adastra**, where the Lustre storage system enabled a more detailed investigation of striping and Direct I/O.

---

## Repository Structure

```text
.
├── bench_posix/       # POSIX I/O benchmarks
├── bench_mpi/         # MPI-IO benchmarks
├── bench_hdf5/        # Parallel HDF5 benchmarks
├── smoke_test/        # Small tests for validating configurations
├── scripts/            # Benchmarking / plotting utilities
├── tutorial/           # Short introduction to running the benchmarks
└── README.md
```

> The exact contents may vary depending on the machine and experiment; the benchmark directories contain the corresponding source code and build/run instructions.

---

## Benchmark Categories

### POSIX I/O

Basic file I/O experiments are used as a reference point for comparing higher-level interfaces.

The benchmarks investigate the effect of:

* data size
* number of processes
* sequential versus parallel access
* standard versus direct I/O

### MPI-IO

MPI-IO benchmarks evaluate both **independent** and **collective** access patterns.

The experiments measure aggregate write bandwidth while varying:

* number of MPI ranks
* amount of data written
* I/O mode
* memory configuration
* filesystem striping

### Parallel HDF5

Parallel HDF5 is tested as a higher-level interface commonly used by scientific applications.

Both standard and Direct I/O configurations are considered, allowing the performance of HDF5-based output to be compared with lower-level MPI-IO approaches.

### Direct I/O

Direct I/O bypasses the operating system page cache and introduces additional requirements on the memory address, offset, and transfer size.

The benchmarks therefore include experiments specifically investigating:

* alignment
* pinned host memory
* transfer size
* MPI rank scaling
* direct MPI-IO
* direct HDF5 I/O

---

## GPU and Host Memory

The GPU experiments use accelerator memory together with host-side buffers to study the data movement involved in writing simulation data to storage.

The code includes experiments with GPU memory transfers and pinned host memory, including configurations designed to overlap:

```text
GPU → Host → Storage
```

and, where applicable, overlap GPU computation and data movement.

Pinned host memory is particularly relevant because it enables more efficient GPU↔CPU transfers, but its allocation must be handled carefully when using many MPI processes since allocations are performed per process.

---

## Adastra and Lustre

A significant part of the project was conducted on **Adastra**, where the Lustre filesystem made it possible to study the impact of filesystem striping.

The experiments vary the Lustre:

* stripe count
* stripe size

and measure the resulting aggregate I/O bandwidth.

This revealed that storage configuration can have a substantial effect on application-level I/O performance, particularly for large parallel writes.

---

## Main Results

The experiments showed that I/O performance depends strongly on the interaction between the application, memory configuration, I/O interface, and filesystem configuration.

Among the main observations:

* Proper memory alignment is essential for Direct I/O.
* Pinned host memory can improve the GPU-to-host transfer path but introduces significant per-process memory requirements.
* MPI-IO can achieve very high aggregate bandwidth when the access pattern and filesystem configuration are well matched.
* Lustre striping has a substantial impact on parallel I/O performance.
* Increasing the stripe count and stripe size can significantly improve the performance of large parallel writes.
* Higher-level interfaces such as HDF5 introduce additional considerations compared with direct MPI-IO.
* Optimizing a synthetic benchmark does not necessarily translate directly into the same improvement for a complete scientific application.

In the final Adastra experiments, aligned MPI Direct I/O reached approximately **81 GB/s** for the tested 4 GB/rank configuration with a `16 × 1 MiB` Lustre stripe configuration.

The same filesystem-level optimization was then evaluated using Smilei. For the tested diagnostic workload, the fraction of the simulation spent in diagnostics decreased from approximately **41% to 17%** when moving from `1 × 1 MiB` to `16 × 4 MiB` striping.

These results illustrate the importance of considering the complete I/O pipeline rather than optimizing GPU transfers or storage independently.

---

## Smilei Validation

To evaluate whether the benchmark results translated to a real scientific workload, the final stage of the project used **Smilei**, a particle-in-cell simulation framework.

The purpose was not to modify Smilei's computational kernels, but to investigate how storage configuration affected the I/O-heavy diagnostic workload.

This provided an application-level validation of the trends observed in the synthetic benchmarks.

---

## Tutorial

A short tutorial is included in the repository to introduce the benchmark workflow and provide examples of how to build and run the tests.

It covers the basic steps required to:

1. Configure the environment
2. Build the benchmarks
3. Run small validation tests
4. Launch MPI/POSIX/HDF5 experiments
5. Collect performance measurements
6. Compare different I/O configurations

The tutorial is intended to make the benchmark suite easier to reuse and adapt to other HPC systems.

---

## Software Environment

The experiments were conducted using HPC environments including:

* **C/C++**
* **CUDA / HIP**
* **MPI**
* **Parallel HDF5**
* **POSIX I/O**
* **Lustre**
* **Smilei**

On Adastra, the experiments used the corresponding Cray programming environment, Cray MPI, parallel HDF5, and ROCm stack available on the system.

---

## Reproducibility

The benchmarks are designed to make individual components of the I/O path measurable independently.

For meaningful comparisons, the following parameters should be recorded for each experiment:

* HPC system and filesystem
* GPU model
* number of GPUs / MPI ranks
* data size per rank
* I/O interface
* collective or independent MPI-IO
* Direct I/O configuration
* memory type
* Lustre stripe count
* Lustre stripe size

Filesystem configuration and system load can significantly affect measured bandwidth, so absolute results should be interpreted in the context of the machine on which they were obtained.

---

## Motivation

The goal of this repository is not to provide a universal "best" I/O configuration. Instead, it provides a collection of reproducible experiments for understanding **where I/O performance is lost and which parts of the GPU-to-storage pipeline are responsible**.

The benchmark suite can therefore be used as a starting point for investigating I/O performance on other GPU-based HPC systems and scientific workloads.

---

## Acknowledgements

This work was carried out during a research internship at **CEA / Maison de la Simulation**.

The project was conducted in the context of high-performance computing and scientific simulation, with experiments performed on the **Ruche** and **Adastra** HPC platforms.

---

## License

[Add your chosen license here.]

