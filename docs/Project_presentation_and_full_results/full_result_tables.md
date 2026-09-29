# Detailed results — supporting data behind plots not tabulated in Intra_node.md

This documents contains the detailed result tables for all runs that led to the results used in `presentation.pdf`.

**A note on scope / assumptions**: for each stripe-configuration sweep, one
table gathers *all* allocations × strategies for that config (rows = transfer
size). D2H bandwidth is omitted from these sweep tables since it isn't stripe-dependent.

# 1 node, 8 GPUs:

## Baseline - no MPI, no HDF5

System and timing:
- 1 node, 8 × AMD Instinct MI250X GCDs visible.
- Results averaged over 20 runs.
- Correctness checks passed for every entry (`OK`).
- Size is per rank, so total size written is `Size*8`.

Stripe layout:
```text
stripe_count:  8
stripe_size:   4194304
pattern:       raid0
stripe_offset: -1
pool:          bandwidth
```

### Pageable allocation results

| Size | D2H ms | D2H GB/s | Individual ms | Individual GB/s | Individual std | Individual OK | pwrite ms | pwrite GB/s | pwrite std | pwrite OK | Mutex ms | Mutex GB/s | Mutex std | Mutex OK |
| ---: | ---: | ---: | ---: | ---: | ---: | :---: | ---: | ---: | ---: | :---: | ---: | ---: | ---: | :---: |
| 1 MB | 0.12 | 8.990 | 35.25 | 0.238 | 6.13 | OK | 3.59 | 2.335 | 0.07 | OK | 7.07 | 1.187 | 0.40 | OK |
| 4 MB | 0.60 | 6.961 | 90.19 | 0.372 | 15.77 | OK | 15.73 | 2.133 | 0.26 | OK | 29.06 | 1.155 | 0.95 | OK |
| 16 MB | 1.91 | 8.767 | 261.64 | 0.513 | 2.92 | OK | 62.78 | 2.138 | 0.89 | OK | 111.07 | 1.208 | 1.57 | OK |
| 64 MB | 6.43 | 10.432 | 963.56 | 0.557 | 11.69 | OK | 247.45 | 2.170 | 2.93 | OK | 416.21 | 1.290 | 3.24 | OK |
| 256 MB | 25.16 | 10.670 | 3652.26 | 0.588 | 63.79 | OK | 943.98 | 2.275 | 12.85 | OK | 1617.87 | 1.327 | 15.32 | OK |
| 1 GB | 80.67 | 13.310 | 13611.76 | 0.631 | 115.32 | OK | 3838.63 | 2.238 | 41.94 | OK | 6420.86 | 1.338 | 37.27 | OK |
| 4 GB | 295.91 | 14.514 | 54456.29 | 0.631 | 2141.65 | OK | 15374.50 | 2.235 | 119.55 | OK | 25334.08 | 1.356 | 159.20 | OK |

### Pinned allocation results

| Size | D2H ms | D2H GB/s | Individual ms | Individual GB/s | Individual std | Individual OK | pwrite ms | pwrite GB/s | pwrite std | pwrite OK | Mutex ms | Mutex GB/s | Mutex std | Mutex OK |
| ---: | ---: | ---: | ---: | ---: | ---: | :---: | ---: | ---: | ---: | :---: | ---: | ---: | ---: | :---: |
| 1 MB | 0.05 | 20.252 | 110.83 | 0.076 | 38.03 | OK | 3.63 | 2.311 | 0.07 | OK | 7.08 | 1.184 | 0.29 | OK |
| 4 MB | 0.21 | 19.889 | 165.09 | 0.203 | 37.89 | OK | 15.68 | 2.141 | 0.21 | OK | 27.84 | 1.205 | 1.25 | OK |
| 16 MB | 0.77 | 21.670 | 397.73 | 0.337 | 81.58 | OK | 62.99 | 2.131 | 0.85 | OK | 108.71 | 1.235 | 1.54 | OK |
| 64 MB | 3.35 | 20.062 | 1178.69 | 0.455 | 63.69 | OK | 233.11 | 2.303 | 3.49 | OK | 411.39 | 1.305 | 2.67 | OK |
| 256 MB | 13.15 | 20.411 | 3941.92 | 0.545 | 558.11 | OK | 955.23 | 2.248 | 12.35 | OK | 1603.08 | 1.340 | 14.77 | OK |
| 1 GB | 55.98 | 19.179 | 14207.54 | 0.605 | 728.25 | OK | 3734.18 | 2.300 | 53.80 | OK | 6354.42 | 1.352 | 50.54 | OK |
| 4 GB | 190.90 | 22.499 | 55054.36 | 0.624 | 832.47 | OK | 15511.27 | 2.215 | 213.89 | OK | 25070.86 | 1.371 | 92.95 | OK |

### Managed allocation results

| Size | D2H ms | D2H GB/s | Individual ms | Individual GB/s | Individual std | Individual OK | pwrite ms | pwrite GB/s | pwrite std | pwrite OK | Mutex ms | Mutex GB/s | Mutex std | Mutex OK |
| ---: | ---: | ---: | ---: | ---: | ---: | :---: | ---: | ---: | ---: | :---: | ---: | ---: | ---: | :---: |
| 1 MB | 0.07 | 14.969 | 91.59 | 0.092 | 28.89 | OK | 3.61 | 2.323 | 0.07 | OK | 7.14 | 1.176 | 0.28 | OK |
| 4 MB | 0.27 | 15.536 | 182.74 | 0.184 | 146.33 | OK | 15.63 | 2.146 | 0.26 | OK | 27.89 | 1.203 | 1.09 | OK |
| 16 MB | 1.08 | 15.476 | 442.07 | 0.304 | 161.42 | OK | 63.39 | 2.117 | 0.73 | OK | 108.54 | 1.237 | 1.78 | OK |
| 64 MB | 4.87 | 13.788 | 986.16 | 0.544 | 36.58 | OK | 231.10 | 2.323 | 2.92 | OK | 407.74 | 1.317 | 1.88 | OK |
| 256 MB | 21.55 | 12.458 | 3969.52 | 0.541 | 412.28 | OK | 979.37 | 2.193 | 10.55 | OK | 1595.92 | 1.346 | 23.13 | OK |
| 1 GB | 69.85 | 15.373 | 14224.01 | 0.604 | 787.70 | OK | 3837.16 | 2.239 | 51.05 | OK | 6362.81 | 1.350 | 40.09 | OK |
| 4 GB | 296.91 | 14.466 | 54112.28 | 0.635 | 548.48 | OK | 15118.44 | 2.273 | 105.93 | OK | 25197.35 | 1.364 | 251.60 | OK |

---

## MPI writes - Lustre default striping

| Strategy | MPI call / behavior | Cache behavior |
|---|---|---|
| Independent | `MPI_File_write_at` with no coordination | Page cache ON |
| Collective | `MPI_File_write_at_all`, ROMIO two-phase I/O | Page cache ON |
| Direct I/O | `MPI_File_write_at` with `direct_io` hint | Cache bypassed |

Stripe layout:
```
stripe_count: 2
stripe_size:  1048576
```

### Pageable allocation results

| Size | D2H ms | D2H GB/s | Independent ms | Independent GB/s | Independent std | Independent OK | Collective ms | Collective GB/s | Collective std | Collective OK | Direct I/O ms | Direct I/O GB/s | Direct I/O std | Direct I/O OK |
|---:|---:|---:|---:|---:|---:|:---:|---:|---:|---:|:---:|---:|---:|---:|:---:|
| 1 MB | 0.14 | 7.647 | 3.40 | 2.469 | 0.06 | OK | 4.01 | 2.092 | 0.37 | OK | 7.39 | 1.135 | 0.40 | OK |
| 4 MB | 0.20 | 20.511 | 12.90 | 2.601 | 0.16 | OK | 17.44 | 1.924 | 0.48 | OK | 12.86 | 2.609 | 0.23 | OK |
| 16 MB | 0.77 | 21.931 | 53.90 | 2.490 | 0.40 | OK | 72.11 | 1.861 | 0.88 | OK | 54.90 | 2.445 | 0.44 | OK |
| 64 MB | 3.03 | 22.182 | 223.93 | 2.397 | 1.59 | OK | 293.46 | 1.829 | 4.88 | OK | 227.62 | 2.359 | 1.44 | OK |
| 256 MB | 12.10 | 22.184 | 902.97 | 2.378 | 9.58 | OK | 1168.42 | 1.838 | 9.62 | OK | 923.54 | 2.325 | 7.70 | OK |
| 1 GB | 48.51 | 22.133 | 3788.02 | 2.268 | 21.61 | OK | 4676.88 | 1.837 | 26.02 | OK | 3695.60 | 2.324 | 39.67 | OK |
| 4 GB | 193.56 | 22.189 | 15279.66 | 2.249 | 92.41 | OK | 18128.34 | 1.895 | 109.60 | OK | 16148.58 | 2.128 | 2179.66 | OK |

### Pinned allocation results

| Size | D2H ms | D2H GB/s | Independent ms | Independent GB/s | Independent std | Independent OK | Collective ms | Collective GB/s | Collective std | Collective OK | Direct I/O ms | Direct I/O GB/s | Direct I/O std | Direct I/O OK |
|---:|---:|---:|---:|---:|---:|:---:|---:|---:|---:|:---:|---:|---:|---:|:---:|
| 1 MB | 0.06 | 18.957 | 3.58 | 2.343 | 0.06 | OK | 3.89 | 2.157 | 0.26 | OK | 3.68 | 2.277 | 2.17 | OK |
| 4 MB | 0.19 | 22.031 | 12.99 | 2.582 | 0.22 | OK | 17.26 | 1.944 | 0.43 | OK | 9.42 | 3.560 | 0.69 | OK |
| 16 MB | 0.75 | 22.337 | 54.96 | 2.442 | 0.36 | OK | 74.71 | 1.797 | 1.20 | OK | 51.48 | 2.607 | 17.10 | OK |
| 64 MB | 3.01 | 22.300 | 221.99 | 2.418 | 2.40 | OK | 290.90 | 1.846 | 2.63 | OK | 127.94 | 4.196 | 2.29 | OK |
| 256 MB | 12.02 | 22.331 | 916.36 | 2.343 | 6.01 | OK | 1152.38 | 1.864 | 9.93 | OK | 440.38 | 4.876 | 29.50 | OK |
| 1 GB | 48.12 | 22.314 | 3727.12 | 2.305 | 37.56 | OK | 4553.15 | 1.887 | 17.61 | OK | 1799.59 | 4.773 | 107.22 | OK |
| 4 GB | 192.43 | 22.320 | 15307.28 | 2.245 | 78.59 | OK | 19288.89 | 1.781 | 2890.91 | OK | 15419.75 | 2.228 | 64.94 | OK |

### Managed allocation results

| Size | D2H ms | D2H GB/s | Independent ms | Independent GB/s | Independent std | Independent OK | Collective ms | Collective GB/s | Collective std | Collective OK | Direct I/O ms | Direct I/O GB/s | Direct I/O std | Direct I/O OK |
|---:|---:|---:|---:|---:|---:|:---:|---:|---:|---:|:---:|---:|---:|---:|:---:|
| 1 MB | 0.05 | 19.275 | 3.55 | 2.360 | 0.07 | OK | 3.75 | 2.240 | 0.21 | OK | 3.45 | 2.430 | 0.93 | OK |
| 4 MB | 0.19 | 22.298 | 12.46 | 2.693 | 0.20 | OK | 17.57 | 1.910 | 0.61 | OK | 9.29 | 3.611 | 0.62 | OK |
| 16 MB | 0.73 | 22.832 | 54.01 | 2.485 | 0.65 | OK | 72.64 | 1.848 | 1.19 | OK | 34.20 | 3.925 | 2.13 | OK |
| 64 MB | 2.92 | 23.007 | 220.84 | 2.431 | 1.93 | OK | 284.82 | 1.885 | 3.17 | OK | 131.02 | 4.098 | 3.06 | OK |
| 256 MB | 11.56 | 23.215 | 916.20 | 2.344 | 6.11 | OK | 1186.19 | 1.810 | 10.49 | OK | 525.75 | 4.085 | 13.49 | OK |
| 1 GB | 46.36 | 23.159 | 3754.92 | 2.288 | 37.19 | OK | 4683.38 | 1.834 | 34.19 | OK | 1943.58 | 4.420 | 127.32 | OK |
| 4 GB | 195.71 | 21.946 | 15373.10 | 2.235 | 74.18 | OK | 18847.80 | 1.823 | 74.62 | OK | 15003.64 | 2.290 | 100.33 | OK |

---

## MPI writes - Dedicated striped directory (16 stripes of size 4MiB)

Stripe layout (directory and inherited files):
```
stripe_count:  16
stripe_size:   4194304
pattern:       raid0
stripe_offset: -1
```

### Pageable allocation results

| Size | D2H ms | D2H GB/s | Independent ms | Independent GB/s | Independent std | Independent OK | Collective ms | Collective GB/s | Collective std | Collective OK | Direct I/O ms | Direct I/O GB/s | Direct I/O std | Direct I/O OK |
|---:|---:|---:|---:|---:|---:|:---:|---:|---:|---:|:---:|---:|---:|---:|:---:|
| 1 MB | 0.14 | 7.596 | 3.79 | 2.213 | 0.05 | OK | 4.57 | 1.834 | 0.43 | OK | 3.83 | 2.189 | 0.05 | OK |
| 4 MB | 0.20 | 20.606 | 16.55 | 2.028 | 0.19 | OK | 16.86 | 1.991 | 0.16 | OK | 16.20 | 2.071 | 0.17 | OK |
| 16 MB | 0.77 | 21.909 | 73.11 | 1.836 | 0.78 | OK | 78.89 | 1.701 | 0.60 | OK | 73.63 | 1.823 | 0.86 | OK |
| 64 MB | 3.03 | 22.171 | 264.70 | 2.028 | 1.81 | OK | 325.52 | 1.649 | 1.41 | OK | 264.02 | 2.033 | 1.29 | OK |
| 256 MB | 12.12 | 22.152 | 1062.03 | 2.022 | 7.67 | OK | 1301.01 | 1.651 | 3.48 | OK | 1063.71 | 2.019 | 133.93 | OK |
| 1 GB | 48.38 | 22.194 | 4270.49 | 2.011 | 24.07 | OK | 5204.00 | 1.651 | 11.93 | OK | 4006.63 | 2.144 | 413.98 | OK |
| 4 GB | 193.49 | 22.197 | 17717.03 | 1.939 | 112.44 | OK | 20661.94 | 1.663 | 20.62 | OK | 17480.18 | 1.966 | 110.71 | OK |

### Pinned allocation results

| Size | D2H ms | D2H GB/s | Independent ms | Independent GB/s | Independent std | Independent OK | Collective ms | Collective GB/s | Collective std | Collective OK | Direct I/O ms | Direct I/O GB/s | Direct I/O std | Direct I/O OK |
|---:|---:|---:|---:|---:|---:|:---:|---:|---:|---:|:---:|---:|---:|---:|:---:|
| 1 MB | 0.05 | 19.126 | 3.79 | 2.211 | 0.03 | OK | 4.50 | 1.865 | 0.31 | OK | 3.02 | 2.777 | 0.29 | OK |
| 4 MB | 0.19 | 21.758 | 16.31 | 2.057 | 0.17 | OK | 17.31 | 1.939 | 0.21 | OK | 4.53 | 7.411 | 1.49 | OK |
| 16 MB | 0.75 | 22.235 | 73.67 | 1.822 | 1.01 | OK | 77.81 | 1.725 | 0.66 | OK | 10.00 | 13.426 | 5.72 | OK |
| 64 MB | 3.01 | 22.258 | 263.16 | 2.040 | 1.88 | OK | 320.56 | 1.675 | 1.15 | OK | 21.14 | 25.400 | 0.95 | OK |
| 256 MB | 12.02 | 22.342 | 1056.81 | 2.032 | 7.30 | OK | 1297.77 | 1.655 | 3.19 | OK | 60.42 | 35.540 | 1.55 | OK |
| 1 GB | 48.21 | 22.272 | 4275.95 | 2.009 | 26.95 | OK | 5225.99 | 1.644 | 9.30 | OK | 213.31 | 40.269 | 4.25 | OK |
| 4 GB | 192.45 | 22.317 | 17383.88 | 1.977 | 123.71 | OK | 21008.50 | 1.636 | 24.55 | OK | 15419.75 | 2.228 | 64.94 | OK |

### Managed allocation results

| Size | D2H ms | D2H GB/s | Independent ms | Independent GB/s | Independent std | Independent OK | Collective ms | Collective GB/s | Collective std | Collective OK | Direct I/O ms | Direct I/O GB/s | Direct I/O std | Direct I/O OK |
|---:|---:|---:|---:|---:|---:|:---:|---:|---:|---:|:---:|---:|---:|---:|:---:|
| 1 MB | 0.05 | 19.267 | 3.80 | 2.205 | 0.04 | OK | 4.44 | 1.891 | 0.36 | OK | 3.09 | 2.717 | 0.24 | OK |
| 4 MB | 0.19 | 22.356 | 16.60 | 2.021 | 0.20 | OK | 17.12 | 1.960 | 0.22 | OK | 4.33 | 7.743 | 0.25 | OK |
| 16 MB | 0.73 | 22.952 | 73.10 | 1.836 | 0.76 | OK | 78.15 | 1.717 | 0.60 | OK | 7.98 | 16.823 | 0.54 | OK |
| 64 MB | 2.88 | 23.328 | 270.28 | 1.986 | 1.63 | OK | 319.89 | 1.678 | 1.04 | OK | 22.17 | 24.216 | 0.91 | OK |
| 256 MB | 11.57 | 23.191 | 1040.47 | 2.064 | 5.91 | OK | 1288.85 | 1.666 | 4.59 | OK | 59.07 | 36.353 | 1.22 | OK |
| 1 GB | 48.57 | 22.106 | 4300.95 | 1.997 | 31.33 | OK | 5154.77 | 1.666 | 12.17 | OK | 294.28 | 29.190 | 61.02 | OK |
| 4 GB | 199.38 | 21.542 | 17329.13 | 1.983 | 162.69 | OK | 20932.90 | 1.641 | 19.40 | OK | 17466.46 | 1.967 | 145.11 | OK |

---

## MPI write in reverse order (4GB -> 1MB), same stripe config as previous run

### Pageable allocation results

| Size | D2H ms | D2H GB/s | Independent ms | Independent GB/s | Independent std | Independent OK | Collective ms | Collective GB/s | Collective std | Collective OK | Direct I/O ms | Direct I/O GB/s | Direct I/O std | Direct I/O OK |
|---:|---:|---:|---:|---:|---:|:---:|---:|---:|---:|:---:|---:|---:|---:|:---:|
| 1 MB | 0.13 | 8.030 | 3.63 | 2.308 | 0.06 | OK | 4.18 | 2.006 | 0.28 | OK | 3.59 | 2.337 | 0.06 | OK |
| 4 MB | 0.22 | 19.505 | 15.50 | 2.164 | 0.19 | OK | 16.19 | 2.072 | 0.13 | OK | 15.90 | 2.110 | 0.26 | OK |
| 16 MB | 0.78 | 21.449 | 62.93 | 2.133 | 0.58 | OK | 71.95 | 1.866 | 0.72 | OK | 64.28 | 2.088 | 0.63 | OK |
| 64 MB | 3.06 | 21.914 | 251.45 | 2.135 | 1.45 | OK | 292.04 | 1.838 | 1.84 | OK | 239.40 | 2.243 | 2.03 | OK |
| 256 MB | 12.09 | 22.198 | 1006.87 | 2.133 | 6.77 | OK | 1187.94 | 1.808 | 3.91 | OK | 1010.22 | 2.126 | 7.26 | OK |
| 1 GB | 48.19 | 22.282 | 3919.51 | 2.192 | 22.37 | OK | 4691.33 | 1.831 | 11.81 | OK | 3871.48 | 2.219 | 33.95 | OK |
| 4 GB | 193.36 | 22.212 | 15772.11 | 2.179 | 83.53 | OK | 18848.17 | 1.823 | 23.59 | OK | 15905.12 | 2.160 | 98.13 | OK |

### Pinned allocation results

| Size | D2H ms | D2H GB/s | Independent ms | Independent GB/s | Independent std | Independent OK | Collective ms | Collective GB/s | Collective std | Collective OK | Direct I/O ms | Direct I/O GB/s | Direct I/O std | Direct I/O OK |
|---:|---:|---:|---:|---:|---:|:---:|---:|---:|---:|:---:|---:|---:|---:|:---:|
| 1 MB | 0.06 | 18.743 | 3.62 | 2.318 | 0.05 | OK | 4.08 | 2.055 | 0.23 | OK | 3.52 | 2.383 | 0.64 | OK |
| 4 MB | 0.19 | 21.608 | 15.74 | 2.131 | 0.21 | OK | 16.16 | 2.076 | 0.24 | OK | 4.77 | 7.034 | 0.67 | OK |
| 16 MB | 0.75 | 22.366 | 63.00 | 2.131 | 0.72 | OK | 72.44 | 1.853 | 0.63 | OK | 9.66 | 13.894 | 0.83 | OK |
| 64 MB | 3.02 | 22.255 | 248.36 | 2.162 | 2.11 | OK | 295.38 | 1.818 | 1.14 | OK | 21.14 | 25.399 | 1.64 | OK |
| 256 MB | 12.03 | 22.317 | 987.37 | 2.175 | 6.06 | OK | 1173.41 | 1.830 | 3.43 | OK | 62.57 | 34.321 | 1.63 | OK |
| 1 GB | 48.12 | 22.312 | 3849.91 | 2.231 | 22.84 | OK | 4780.38 | 1.797 | 12.62 | OK | 234.46 | 36.637 | 2.94 | OK |
| 4 GB | 196.57 | 21.850 | 16237.61 | 2.116 | 117.65 | OK | 19137.85 | 1.795 | 25.56 | OK | 15888.73 | 2.163 | 99.85 | OK |

### Managed allocation results

| Size | D2H ms | D2H GB/s | Independent ms | Independent GB/s | Independent std | Independent OK | Collective ms | Collective GB/s | Collective std | Collective OK | Direct I/O ms | Direct I/O GB/s | Direct I/O std | Direct I/O OK |
|---:|---:|---:|---:|---:|---:|:---:|---:|---:|---:|:---:|---:|---:|---:|:---:|
| 1 MB | 0.05 | 19.244 | 3.63 | 2.311 | 0.05 | OK | 4.18 | 2.006 | 0.35 | OK | 3.09 | 2.715 | 0.32 | OK |
| 4 MB | 0.19 | 21.900 | 15.74 | 2.131 | 0.18 | OK | 16.11 | 2.083 | 0.28 | OK | 4.41 | 7.609 | 0.67 | OK |
| 16 MB | 0.74 | 22.728 | 62.99 | 2.131 | 0.79 | OK | 70.08 | 1.915 | 0.66 | OK | 9.89 | 13.567 | 0.71 | OK |
| 64 MB | 2.90 | 23.173 | 244.75 | 2.194 | 1.77 | OK | 296.78 | 1.809 | 1.34 | OK | 22.05 | 24.348 | 1.19 | OK |
| 256 MB | 11.70 | 22.939 | 979.68 | 2.192 | 10.74 | OK | 1180.10 | 1.820 | 3.14 | OK | 63.38 | 33.883 | 4.13 | OK |
| 1 GB | 46.65 | 23.018 | 3834.70 | 2.240 | 27.80 | OK | 4575.24 | 1.877 | 9.65 | OK | 239.54 | 35.860 | 3.90 | OK |
| 4 GB | 194.26 | 22.109 | 16513.21 | 2.081 | 1874.59 | OK | 19153.91 | 1.794 | 725.88 | OK | 15794.33 | 2.175 | 72.12 | OK |

---

## Speedup analysis — best no-MPI vs best MPI (per-size detail)

### Pageable

| Size | Best no-MPI GB/s | Winning no-MPI path | Best MPI GB/s | Winning MPI run/strategy | Speedup |
|---:|---:|---|---:|---|---:|
| 1 MB | 2.335 | pwrite | 2.469 | stripe2x1m/Independent | 1.06x |
| 4 MB | 2.133 | pwrite | 2.609 | stripe2x1m/Direct I/O | 1.22x |
| 16 MB | 2.138 | pwrite | 2.490 | stripe2x1m/Independent | 1.16x |
| 64 MB | 2.170 | pwrite | 2.397 | stripe2x1m/Independent | 1.10x |
| 256 MB | 2.275 | pwrite | 2.378 | stripe2x1m/Independent | 1.05x |
| 1 GB | 2.238 | pwrite | 2.324 | stripe2x1m/Direct I/O | 1.04x |
| 4 GB | 2.235 | pwrite | 2.249 | stripe2x1m/Independent | 1.01x |

### Pinned

| Size | Best no-MPI GB/s | Winning no-MPI path | Best MPI GB/s | Winning MPI run/strategy | Speedup |
|---:|---:|---|---:|---|---:|
| 1 MB | 2.311 | pwrite | 2.777 | stripe16x4m/Direct I/O | 1.20x |
| 4 MB | 2.141 | pwrite | 7.411 | stripe16x4m/Direct I/O | 3.46x |
| 16 MB | 2.131 | pwrite | 13.894 | stripe16x4m_rev/Direct I/O | 6.52x |
| 64 MB | 2.303 | pwrite | 25.400 | stripe16x4m/Direct I/O | 11.03x |
| 256 MB | 2.248 | pwrite | 35.540 | stripe16x4m/Direct I/O | 15.81x |
| 1 GB | 2.300 | pwrite | 40.269 | stripe16x4m/Direct I/O | 17.51x |
| 4 GB | 2.215 | pwrite | 2.245 | stripe2x1m/Independent | 1.01x |

### Managed

| Size | Best no-MPI GB/s | Winning no-MPI path | Best MPI GB/s | Winning MPI run/strategy | Speedup |
|---:|---:|---|---:|---|---:|
| 1 MB | 2.323 | pwrite | 2.717 | stripe16x4m/Direct I/O | 1.17x |
| 4 MB | 2.146 | pwrite | 7.743 | stripe16x4m/Direct I/O | 3.61x |
| 16 MB | 2.117 | pwrite | 16.823 | stripe16x4m/Direct I/O | 7.95x |
| 64 MB | 2.323 | pwrite | 24.348 | stripe16x4m_rev/Direct I/O | 10.48x |
| 256 MB | 2.193 | pwrite | 36.353 | stripe16x4m/Direct I/O | 16.58x |
| 1 GB | 2.239 | pwrite | 35.860 | stripe16x4m_rev/Direct I/O | 16.02x |
| 4 GB | 2.273 | pwrite | 2.290 | stripe2x1m/Direct I/O | 1.01x |

---

## HDF5 writes - testing 8 stripe configurations

### stripe count=8, size=1 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 0.79 | 0.71 | 1.01 | 0.76 | 0.77 | 1.02 | 0.79 | 0.71 | 1.02 |
| 16 MB | 1.11 | 0.99 | 1.23 | 1.11 | 0.99 | 1.22 | 1.18 | 0.97 | 1.23 |
| 64 MB | 1.32 | 1.20 | 1.57 | 1.31 | 1.20 | 1.56 | 1.33 | 1.20 | 1.59 |
| 256 MB | 1.34 | 1.28 | 1.70 | 1.29 | 1.27 | 1.71 | 1.24 | 1.29 | 1.71 |
| 1 GB | 1.49 | 1.28 | 1.71 | 1.45 | 1.31 | 1.72 | 1.52 | 1.31 | 1.71 |
| 4 GB | 1.51 | 1.28 | 1.62 | 1.52 | 1.27 | 1.63 | 1.46 | 1.30 | 1.64 |

### stripe count=8, size=2 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 0.77 | 0.78 | 0.98 | 0.79 | 0.83 | 0.93 | 0.82 | 0.82 | 0.92 |
| 16 MB | 1.12 | 1.00 | 1.25 | 1.13 | 1.00 | 1.26 | 1.08 | 1.00 | 1.27 |
| 64 MB | 1.34 | 1.19 | 1.54 | 1.22 | 1.18 | 1.59 | 1.30 | 1.20 | 1.58 |
| 256 MB | 1.33 | 1.27 | 1.70 | 1.30 | 1.28 | 1.71 | 1.31 | 1.29 | 1.72 |
| 1 GB | 1.53 | 1.32 | 1.73 | 1.46 | 1.29 | 1.73 | 1.45 | 1.32 | 1.75 |
| 4 GB | 1.48 | 1.29 | 1.66 | 1.48 | 1.28 | 1.66 | 1.47 | 1.30 | 1.72 |

### stripe count=8, size=4 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 0.75 | 0.56 | 0.73 | 0.72 | 0.64 | 0.79 | 0.72 | 0.66 | 0.81 |
| 16 MB | 1.18 | 1.12 | 1.63 | 1.22 | 1.12 | 1.66 | 1.26 | 1.13 | 1.64 |
| 64 MB | 1.33 | 1.21 | 1.70 | 1.29 | 1.19 | 1.74 | 1.36 | 1.22 | 1.70 |
| 256 MB | 1.26 | 1.22 | 1.74 | 1.43 | 1.23 | 1.75 | 1.40 | 1.26 | 1.70 |
| 1 GB | 1.56 | 1.25 | 1.79 | 1.59 | 1.24 | 1.77 | 1.58 | 1.26 | 1.76 |
| 4 GB | 1.60 | 1.25 | 1.64 | 1.51 | 1.23 | 1.65 | 1.50 | 1.22 | 1.64 |

### stripe count=16, size=1 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 0.71 | 0.66 | 0.88 | 0.70 | 0.71 | 0.95 | 0.74 | 0.72 | 0.94 |
| 16 MB | 1.27 | 1.04 | 1.40 | 1.19 | 1.05 | 1.38 | 1.18 | 1.05 | 1.41 |
| 64 MB | 1.27 | 1.13 | 1.49 | 1.22 | 1.13 | 1.52 | 1.29 | 1.13 | 1.51 |
| 256 MB | 1.27 | 1.19 | 1.57 | 1.29 | 1.19 | 1.56 | 1.18 | 1.18 | 1.54 |
| 1 GB | 1.39 | 1.18 | 1.58 | 1.39 | 1.18 | 1.54 | 1.38 | 1.18 | 1.54 |
| 4 GB | 1.37 | 1.15 | 1.42 | 1.41 | 1.15 | 1.43 | 1.39 | 0.99 | 1.45 |

### stripe count=16, size=2 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 0.71 | 0.74 | 0.86 | 0.77 | 0.78 | 0.91 | 0.76 | 0.79 | 0.94 |
| 16 MB | 1.20 | 1.07 | 1.42 | 1.20 | 1.07 | 1.45 | 1.17 | 1.07 | 1.44 |
| 64 MB | 1.22 | 1.16 | 1.52 | 1.25 | 1.16 | 1.54 | 1.29 | 1.16 | 1.55 |
| 256 MB | 1.34 | 1.20 | 1.59 | 1.27 | 1.19 | 1.59 | 1.28 | 1.19 | 1.59 |
| 1 GB | 1.43 | 1.19 | 1.57 | 1.44 | 1.18 | 1.56 | 1.41 | 1.18 | 1.58 |
| 4 GB | 1.40 | 1.16 | 1.43 | 1.39 | 1.17 | 1.47 | 1.39 | 1.16 | 1.47 |

### stripe count=16, size=4 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 0.71 | 0.57 | 0.76 | 0.72 | 0.63 | 0.79 | 0.73 | 0.61 | 0.77 |
| 16 MB | 1.08 | 1.07 | 1.52 | 1.11 | 1.05 | 1.52 | 1.11 | 1.06 | 1.52 |
| 64 MB | 1.36 | 1.15 | 1.57 | 1.27 | 1.14 | 1.59 | 1.24 | 1.16 | 1.61 |
| 256 MB | 1.33 | 1.15 | 1.62 | 1.30 | 1.16 | 1.61 | 1.33 | 1.14 | 1.63 |
| 1 GB | 1.57 | 1.17 | 1.61 | 1.59 | 1.14 | 1.60 | 1.57 | 1.14 | 1.61 |
| 4 GB | 1.51 | 1.13 | 1.49 | 1.56 | 1.15 | 1.48 | 1.57 | 1.12 | 1.47 |

### stripe count=24, size=1 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 0.71 | 0.65 | 0.82 | 0.67 | 0.70 | 0.91 | 0.70 | 0.70 | 0.80 |
| 16 MB | 0.98 | 0.90 | 1.09 | 0.99 | 0.90 | 1.08 | 0.99 | 0.90 | 1.08 |
| 64 MB | 1.12 | 1.01 | 1.21 | 1.09 | 1.02 | 1.21 | 1.10 | 1.02 | 1.21 |
| 256 MB | 1.16 | 1.08 | 1.33 | 1.09 | 1.08 | 1.33 | 1.15 | 1.08 | 1.33 |
| 1 GB | 1.30 | 1.09 | 1.36 | 1.30 | 1.09 | 1.36 | 1.31 | 1.10 | 1.37 |
| 4 GB | 1.32 | 1.06 | 1.33 | 1.34 | 1.06 | 1.34 | 1.39 | 1.07 | 1.33 |

### stripe count=24, size=2 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 0.63 | 0.68 | 0.83 | 0.74 | 0.71 | 0.81 | 0.71 | 0.76 | 0.80 |
| 16 MB | 0.99 | 0.97 | 1.25 | 1.05 | 0.98 | 1.26 | 1.07 | 0.98 | 1.27 |
| 64 MB | 1.16 | 1.08 | 1.39 | 1.14 | 1.08 | 1.41 | 1.17 | 1.09 | 1.40 |
| 256 MB | 1.24 | 1.12 | 1.46 | 1.21 | 1.13 | 1.45 | 1.20 | 1.13 | 1.46 |
| 1 GB | 1.48 | 1.13 | 1.49 | 1.45 | 1.13 | 1.45 | 1.45 | 1.13 | 1.48 |
| 4 GB | 1.52 | 1.07 | 1.41 | 1.45 | 1.09 | 1.38 | 1.48 | 1.08 | 1.42 |

### Lustre default layout: 2 stripes 1MiB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 0.70 | 0.58 | 0.76 | 0.70 | 0.63 | 0.77 | 0.70 | 0.65 | 0.81 |
| 4 MB | 1.02 | 0.92 | 1.27 | 1.01 | 0.89 | 1.22 | 1.03 | 0.90 | 1.22 |
| 16 MB | 1.20 | 1.13 | 1.66 | 1.21 | 1.12 | 1.64 | 1.19 | 1.13 | 1.67 |
| 64 MB | 1.33 | 1.22 | 1.73 | 1.40 | 1.20 | 1.70 | 1.28 | 1.20 | 1.68 |
| 256 MB | 1.35 | 1.23 | 1.76 | 1.28 | 1.22 | 1.72 | 1.39 | 1.22 | 1.72 |
| 1 GB | 1.62 | 1.24 | 1.77 | 1.56 | 1.23 | 1.76 | 1.58 | 1.26 | 1.76 |
| 4 GB | 1.54 | 1.24 | 1.64 | 1.56 | 1.22 | 1.68 | 1.57 | 1.21 | 1.66 |


---

# 2 nodes, 16 GPUs 

## HDF5 stripe sweep — detailed per-config tables

Three allocations (Pageable / Pinned / Managed) × three HDF5 write strategies
(Independent / Collective / Chunked+Collective), one table per swept stripe
configuration. All values in GB/s.

### stripe count=8, size=1 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 0.730 | 1.490 | 1.350 | 0.770 | 1.550 | 1.250 | 0.730 | 1.530 | 1.330 |
| 4 MB | 1.070 | 2.120 | 2.050 | 1.040 | 2.110 | 2.040 | 1.040 | 2.080 | 1.970 |
| 16 MB | 1.230 | 2.470 | 2.350 | 1.260 | 2.350 | 2.310 | 1.200 | 2.500 | 2.330 |
| 64 MB | 2.060 | 2.570 | 2.550 | 2.060 | 2.550 | 2.430 | 2.030 | 2.340 | 2.400 |
| 256 MB | 2.400 | 2.330 | 2.410 | 2.390 | 2.420 | 2.440 | 2.310 | 2.710 | 2.420 |
| 1 GB | 2.790 | 2.640 | 2.720 | 2.930 | 2.660 | 2.710 | 2.890 | 2.630 | 2.740 |
| 4 GB | 2.530 | 1.990 | 2.070 | 2.520 | 2.190 | 2.160 | 2.550 | 2.240 | 2.170 |

### stripe count=8, size=2 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 1.200 | 1.470 | 1.400 | 1.200 | 1.580 | 1.360 | 1.130 | 1.540 | 1.380 |
| 4 MB | 1.120 | 2.020 | 1.890 | 1.060 | 2.000 | 1.660 | 1.130 | 2.020 | 1.720 |
| 16 MB | 1.200 | 2.340 | 2.440 | 1.190 | 2.340 | 2.540 | 1.190 | 2.330 | 2.380 |
| 64 MB | 2.020 | 2.240 | 2.520 | 2.050 | 2.280 | 2.510 | 2.030 | 2.240 | 2.540 |
| 256 MB | 2.430 | 2.530 | 2.650 | 2.340 | 2.380 | 2.690 | 2.460 | 2.580 | 2.660 |
| 1 GB | 2.800 | 2.540 | 2.170 | 2.900 | 2.520 | 2.410 | 2.850 | 2.540 | 2.400 |
| 4 GB | 2.530 | 2.270 | 2.080 | 2.510 | 2.340 | 2.060 | 2.560 | 2.270 | 2.100 |

### stripe count=8, size=4 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 0.970 | 1.210 | 1.230 | 0.970 | 1.300 | 1.220 | 1.010 | 1.300 | 1.220 |
| 4 MB | 1.020 | 2.110 | 1.830 | 1.060 | 2.090 | 1.820 | 1.020 | 2.060 | 1.910 |
| 16 MB | 1.390 | 2.430 | 2.350 | 1.350 | 2.340 | 2.320 | 1.430 | 2.430 | 2.380 |
| 64 MB | 1.920 | 2.520 | 2.560 | 1.960 | 2.510 | 2.720 | 1.900 | 2.580 | 2.550 |
| 256 MB | 2.300 | 2.530 | 2.600 | 2.350 | 2.620 | 2.480 | 2.310 | 2.590 | 2.520 |
| 1 GB | 2.880 | 2.580 | 1.910 | 2.950 | 2.660 | 1.900 | 2.930 | 2.590 | 1.920 |
| 4 GB | 2.530 | 2.300 | 2.140 | 2.550 | 2.390 | 2.120 | 2.530 | 2.380 | 2.150 |

### stripe count=16, size=1 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 1.310 | 1.540 | 1.130 | 1.180 | 1.560 | 1.700 | 1.310 | 1.560 | 1.690 |
| 4 MB | 1.110 | 1.870 | 1.900 | 1.030 | 1.850 | 2.160 | 1.060 | 1.850 | 2.000 |
| 16 MB | 1.080 | 2.230 | 2.470 | 1.070 | 2.270 | 2.470 | 1.080 | 2.230 | 2.390 |
| 64 MB | 1.740 | 2.050 | 2.530 | 1.740 | 2.170 | 2.500 | 1.700 | 2.200 | 2.550 |
| 256 MB | 2.220 | 2.350 | 2.580 | 2.070 | 2.390 | 2.620 | 2.130 | 2.390 | 2.620 |
| 1 GB | 2.740 | 2.360 | 2.770 | 2.640 | 2.340 | 2.730 | 2.650 | 2.370 | 2.740 |
| 4 GB | 2.390 | 1.830 | 2.070 | 2.430 | 1.830 | 2.010 | 2.440 | 1.810 | 2.070 |

### stripe count=16, size=2 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 1.180 | 1.360 | 1.510 | 1.110 | 1.480 | 1.490 | 1.150 | 1.480 | 1.500 |
| 4 MB | 0.910 | 1.820 | 1.700 | 0.970 | 1.830 | 1.740 | 0.980 | 1.830 | 1.800 |
| 16 MB | 1.290 | 2.210 | 2.710 | 1.230 | 2.190 | 2.600 | 1.240 | 2.230 | 2.620 |
| 64 MB | 1.550 | 2.120 | 2.600 | 1.640 | 2.120 | 2.600 | 1.630 | 2.160 | 2.590 |
| 256 MB | 2.090 | 2.330 | 2.680 | 2.160 | 2.350 | 2.680 | 2.160 | 2.310 | 2.700 |
| 1 GB | 2.740 | 2.310 | 2.610 | 2.680 | 2.330 | 2.620 | 2.680 | 2.340 | 2.640 |
| 4 GB | 2.400 | 2.130 | 2.040 | 2.380 | 2.110 | 2.050 | 2.390 | 2.110 | 2.010 |

### stripe count=16, size=4 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 0.950 | 1.130 | 1.110 | 0.980 | 1.210 | 1.160 | 1.000 | 1.240 | 1.160 |
| 4 MB | 1.680 | 1.850 | 2.100 | 1.680 | 1.820 | 2.180 | 1.690 | 1.840 | 2.150 |
| 16 MB | 1.350 | 2.140 | 2.410 | 1.470 | 2.150 | 2.540 | 1.400 | 2.140 | 2.620 |
| 64 MB | 1.640 | 2.190 | 2.700 | 1.580 | 2.230 | 2.710 | 1.660 | 2.080 | 2.670 |
| 256 MB | 2.020 | 2.210 | 2.610 | 2.120 | 2.190 | 2.680 | 2.120 | 2.220 | 2.640 |
| 1 GB | 2.780 | 2.210 | 2.580 | 2.680 | 2.220 | 2.610 | 2.730 | 2.210 | 2.570 |
| 4 GB | 2.450 | 2.120 | 2.050 | 2.460 | 2.070 | 1.940 | 2.440 | 2.110 | 2.010 |

### stripe count=24, size=1 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 0.900 | 1.150 | 1.530 | 1.070 | 1.400 | 1.660 | 1.040 | 1.010 | 1.640 |
| 4 MB | 0.880 | 1.730 | 1.860 | 0.980 | 1.790 | 1.980 | 0.880 | 1.460 | 1.850 |
| 16 MB | 1.210 | 2.050 | 2.280 | 1.200 | 2.080 | 2.300 | 1.250 | 2.090 | 2.390 |
| 64 MB | 1.710 | 1.990 | 2.290 | 1.660 | 2.010 | 2.330 | 1.600 | 2.000 | 2.360 |
| 256 MB | 2.030 | 2.230 | 2.420 | 2.000 | 2.220 | 2.380 | 2.070 | 2.230 | 2.500 |
| 1 GB | 2.660 | 2.230 | 2.400 | 2.670 | 2.190 | 2.410 | 2.630 | 2.210 | 2.390 |
| 4 GB | 2.240 | 1.710 | 1.680 | 2.260 | 1.510 | 1.690 | 2.310 | 1.570 | 1.690 |

### stripe count=24, size=2 MB

| Size | Pageable Ind | Pageable Coll | Pageable Chunk+Coll | Pinned Ind | Pinned Coll | Pinned Chunk+Coll | Managed Ind | Managed Coll | Managed Chunk+Coll |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 1.110 | 1.290 | 1.240 | 0.860 | 1.240 | 1.500 | 1.110 | 1.410 | 1.490 |
| 4 MB | 1.170 | 1.750 | 1.760 | 1.190 | 1.690 | 1.730 | 1.160 | 1.730 | 1.820 |
| 16 MB | 1.220 | 1.790 | 1.960 | 1.140 | 1.760 | 1.950 | 1.090 | 1.770 | 1.980 |
| 64 MB | 1.640 | 1.920 | 2.210 | 1.600 | 1.870 | 2.210 | 1.480 | 1.890 | 2.230 |
| 256 MB | 1.860 | 2.180 | 2.480 | 1.820 | 2.170 | 2.400 | 1.880 | 2.180 | 2.430 |
| 1 GB | 2.450 | 2.220 | 1.800 | 2.410 | 2.220 | 1.800 | 2.440 | 2.210 | 2.420 |
| 4 GB | 2.270 | 2.020 | 1.880 | 2.220 | 1.930 | 1.880 | 2.220 | 2.020 | 1.870 |

### Lustre default layout (stripe count=2, size=1 MB) — Pinned memory only

| Size | Independent GB/s | Collective GB/s |
|---:|---:|---:|
| 1 MB | 0.72 | 0.97 |
| 4 MB | 1.09 | 1.53 |
| 16 MB | 1.74 | 1.69 |
| 64 MB | 2.24 | 1.63 |
| 256 MB | 2.53 | 1.70 |
| 1 GB | 2.98 | 1.62 |
| 4 GB | 2.56 | 1.57 |

### Stripe configurations for 16GB/rank, pinned memory only

| Stripe config | Independent GB/s | Collective GB/s |
|---|---:|---:|
| count=2,  size=1 MB (default) | 2.28 | 1.05 |
| count=8,  size=1 MB | 2.21 | 1.06 |
| count=8,  size=4 MB | **2.22** | **2.09** |
| count=16, size=1 MB | 2.25 | 0.93 |
| count=16, size=4 MB | 2.24 | 1.90 |

---

## Intra-node (16 ranks) MPI-IO full stripe sweep — detailed per-config tables

Three allocations (Pageable / Pinned / Managed) × three MPI-IO write
strategies (Independent / Collective / Direct I/O), one table per swept
stripe configuration, plus the separately-collected Lustre-default layout.
All values in GB/s.

### stripe count=8, size=1 MB

| Size | Pageable Ind | Pageable Coll | Pageable DIO | Pinned Ind | Pinned Coll | Pinned DIO | Managed Ind | Managed Coll | Managed DIO |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 1.034 | 1.839 | 1.715 | 1.030 | 1.908 | 7.228 | 0.993 | 1.852 | 8.974 |
| 4 MB | 2.624 | 2.789 | 2.624 | 2.589 | 2.809 | 13.585 | 2.543 | 2.802 | 13.163 |
| 16 MB | 2.814 | 3.367 | 2.904 | 2.824 | 3.363 | 17.365 | 2.892 | 3.286 | 18.104 |
| 64 MB | 3.203 | 3.531 | 3.327 | 3.372 | 3.585 | 22.839 | 3.374 | 3.560 | 23.008 |
| 256 MB | 3.835 | 3.584 | 3.839 | 3.836 | 3.636 | 24.958 | 3.806 | 3.586 | 25.237 |
| 1 GB | 3.921 | 3.618 | 3.876 | 3.888 | 3.608 | 21.063 | 3.925 | 3.651 | 22.367 |
| 4 GB | 4.007 | 3.583 | 3.955 | 3.991 | 3.659 | 4.038 | 4.005 | 3.655 | 4.000 |

### stripe count=8, size=2 MB

| Size | Pageable Ind | Pageable Coll | Pageable DIO | Pinned Ind | Pinned Coll | Pinned DIO | Managed Ind | Managed Coll | Managed DIO |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 2.218 | 1.819 | 2.744 | 2.289 | 1.803 | 7.354 | 2.265 | 1.880 | 7.417 |
| 4 MB | 2.530 | 2.789 | 2.624 | 2.589 | 2.809 | 13.585 | 2.458 | 2.832 | 13.163 |
| 16 MB | 2.913 | 3.271 | 2.940 | 2.900 | 3.369 | 19.647 | 2.931 | 3.286 | 20.256 |
| 64 MB | 3.510 | 3.640 | 3.739 | 3.704 | 3.641 | 25.267 | 3.669 | 3.614 | 26.248 |
| 256 MB | 4.081 | 3.735 | 4.022 | 3.888 | 3.717 | 31.202 | 4.071 | 3.701 | 30.484 |
| 1 GB | 4.121 | 3.772 | 4.154 | 4.077 | 3.718 | 27.759 | 4.128 | 3.758 | 27.213 |
| 4 GB | 4.114 | 3.732 | 4.074 | 4.160 | 3.777 | 4.099 | 4.096 | 3.711 | 4.123 |

### stripe count=8, size=4 MB

| Size | Pageable Ind | Pageable Coll | Pageable DIO | Pinned Ind | Pinned Coll | Pinned DIO | Managed Ind | Managed Coll | Managed DIO |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 2.143 | 1.659 | 2.158 | 2.183 | 1.734 | 5.156 | 2.192 | 1.729 | 5.108 |
| 4 MB | 1.140 | 2.673 | 1.870 | 1.158 | 2.733 | 12.264 | 1.149 | 2.743 | 9.777 |
| 16 MB | 3.374 | 3.443 | 3.394 | 3.062 | 3.441 | 23.772 | 3.480 | 3.395 | 16.391 |
| 64 MB | 4.068 | 3.616 | 4.080 | 4.010 | 3.516 | 44.922 | 4.011 | 3.462 | 45.048 |
| 256 MB | 4.286 | 3.588 | 4.154 | 4.285 | 3.589 | 62.596 | 4.388 | 3.530 | 43.876 |
| 1 GB | 4.212 | 3.692 | 4.228 | 4.298 | 3.592 | 48.833 | 4.275 | 3.666 | 58.524 |
| 4 GB | 4.254 | 3.711 | 4.178 | 4.273 | 3.707 | 4.173 | 4.386 | 3.752 | 4.241 |

### stripe count=16, size=1 MB

| Size | Pageable Ind | Pageable Coll | Pageable DIO | Pinned Ind | Pinned Coll | Pinned DIO | Managed Ind | Managed Coll | Managed DIO |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 2.191 | 1.712 | 5.397 | 2.064 | 1.759 | 6.197 | 2.193 | 1.786 | 8.996 |
| 4 MB | 2.380 | 2.333 | 2.438 | 2.422 | 2.296 | 21.192 | 2.438 | 2.247 | 21.729 |
| 16 MB | 2.729 | 2.861 | 2.704 | 2.716 | 2.918 | 24.344 | 2.729 | 2.912 | 24.290 |
| 64 MB | 3.051 | 3.072 | 3.044 | 3.079 | 3.127 | 39.195 | 3.091 | 3.090 | 38.457 |
| 256 MB | 3.405 | 3.190 | 3.386 | 3.415 | 3.184 | 43.023 | 3.419 | 3.154 | 50.416 |
| 1 GB | 3.524 | 3.220 | 3.494 | 3.522 | 3.260 | 43.137 | 3.469 | 3.175 | 37.711 |
| 4 GB | 3.473 | 3.217 | 3.517 | 3.491 | 3.250 | 3.433 | 3.503 | 3.195 | 3.442 |

### stripe count=16, size=4 MB

| Size | Pageable Ind | Pageable Coll | Pageable DIO | Pinned Ind | Pinned Coll | Pinned DIO | Managed Ind | Managed Coll | Managed DIO |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 2.045 | 1.535 | 2.114 | 2.134 | 1.521 | 4.593 | 2.078 | 1.577 | 4.477 |
| 4 MB | 2.413 | 2.107 | 2.461 | 2.419 | 1.895 | 14.822 | 2.435 | 2.125 | 13.029 |
| 16 MB | 2.597 | 2.414 | 2.550 | 2.557 | 2.391 | 25.509 | 2.540 | 2.413 | 18.420 |
| 64 MB | 2.801 | 3.026 | 2.810 | 2.825 | 3.007 | 32.633 | 2.850 | 3.004 | 32.392 |
| 256 MB | 3.108 | 3.245 | 3.086 | 3.126 | 3.238 | 48.188 | 3.034 | 3.247 | 47.919 |
| 1 GB | 3.509 | 3.310 | 3.522 | 3.548 | 3.350 | 47.950 | 3.574 | 3.259 | 47.553 |
| 4 GB | 3.749 | 3.338 | 3.697 | 3.727 | 3.302 | 3.749 | 3.730 | 3.288 | 3.717 |

### stripe count=24, size=1 MB

| Size | Pageable Ind | Pageable Coll | Pageable DIO | Pinned Ind | Pinned Coll | Pinned DIO | Managed Ind | Managed Coll | Managed DIO |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 2.007 | 1.739 | 3.270 | 2.054 | 1.744 | 5.651 | 2.039 | 1.751 | 8.429 |
| 4 MB | 1.894 | 2.106 | 1.892 | 1.959 | 2.125 | 22.812 | 1.891 | 2.109 | 23.661 |
| 16 MB | 2.399 | 2.683 | 2.340 | 2.378 | 2.736 | 21.843 | 2.404 | 2.682 | 28.918 |
| 64 MB | 2.731 | 2.872 | 2.745 | 2.738 | 2.877 | 49.948 | 2.623 | 2.878 | 34.555 |
| 256 MB | 3.135 | 2.930 | 3.150 | 3.186 | 2.955 | 30.998 | 3.177 | 2.932 | 30.845 |
| 1 GB | 3.330 | 2.995 | 3.321 | 3.299 | 2.968 | 75.207 | 3.323 | 2.966 | 58.055 |
| 4 GB | 3.315 | 3.011 | 3.292 | 3.330 | 2.977 | 3.293 | 3.387 | 2.990 | 3.291 |

### stripe count=24, size=2 MB

| Size | Pageable Ind | Pageable Coll | Pageable DIO | Pinned Ind | Pinned Coll | Pinned DIO | Managed Ind | Managed Coll | Managed DIO |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 2.045 | 1.703 | 2.894 | 2.100 | 1.574 | 6.802 | 2.063 | 1.623 | 6.849 |
| 4 MB | 1.738 | 2.170 | 1.737 | 1.771 | 2.157 | 19.884 | 1.722 | 2.204 | 20.503 |
| 16 MB | 2.430 | 2.721 | 2.210 | 2.404 | 2.771 | 31.448 | 2.354 | 2.716 | 30.513 |
| 64 MB | 2.850 | 2.899 | 2.905 | 2.845 | 2.901 | 30.837 | 2.808 | 2.895 | 28.937 |
| 256 MB | 3.270 | 2.943 | 3.274 | 3.252 | 2.951 | 75.599 | 3.271 | 2.966 | 60.374 |
| 1 GB | 3.375 | 2.980 | 3.376 | 3.384 | 2.985 | 52.817 | 3.410 | 3.018 | 51.964 |
| 4 GB | 3.373 | 2.979 | 3.434 | 3.413 | 3.010 | 3.336 | 3.300 | 2.993 | 3.339 |

**Note:** `count=16, size=2 MB` was cancelled due to the job time limit and
`count=24, size=4 MB` was never tested.

### Lustre default layout (stripe count=2, size=1 MB) — 16 ranks

| Size | Pageable Ind | Pageable Coll | Pageable DIO | Pinned Ind | Pinned Coll | Pinned DIO | Managed Ind | Managed Coll | Managed DIO |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 MB | 1.935 | 1.529 | 2.188 | 1.911 | 1.404 | 4.213 | 1.996 | 1.424 | 4.401 |
| 4 MB | 2.757 | 1.939 | 2.950 | 2.733 | 1.997 | 5.704 | 2.655 | 2.030 | 6.093 |
| 16 MB | 3.206 | 2.339 | 3.106 | 3.155 | 2.337 | 5.934 | 3.198 | 2.386 | 5.881 |
| 64 MB | 4.240 | 2.412 | 4.207 | 4.213 | 2.465 | 5.712 | 4.240 | 2.469 | 5.547 |
| 256 MB | 4.402 | 2.477 | 4.431 | 4.497 | 2.535 | 7.079 | 4.400 | 2.557 | 5.415 |
| 1 GB | 4.374 | 2.460 | 4.401 | 4.409 | 2.526 | 6.967 | 4.443 | 2.538 | 8.193 |
| 4 GB | 4.457 | 2.472 | 4.461 | 4.477 | 2.501 | 4.524 | 4.474 | 2.564 | 4.421 |

### MPI Direct I/O memory-aligned stripe-sweep on 4GB

| Stripe count | Stripe size | D2H time (ms) | D2H bandwidth (GB/s) | Independent time (ms) | Independent bandwidth (GB/s) | Independent std. (ms) | Collective time (ms) | Collective bandwidth (GB/s) | Collective std. (ms) | Direct I/O time (ms) | Direct I/O bandwidth (GB/s) | Direct I/O std. (ms) |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 2 | 1 MiB | 194.55 | 22.076 | 17,123.21 | 4.013 | 223.90 | 18,463.92 | 3.722 | 35.28 | 2,944.71 | 23.337 | 110.57 |
| 8 | 1 MiB | 190.90 | 22.498 | 16,404.79 | 4.189 | 173.63 | 17,694.04 | 3.884 | 54.13 | 2,402.88 | 28.599 | 197.57 |
| 8 | 4 MiB | 193.23 | 22.228 | 19,625.75 | 3.501 | 361.18 | 21,120.57 | 3.254 | 26.92 | 1,821.50 | 37.727 | 146.27 |
| 16 | 1 MiB | 192.57 | 22.303 | 17,725.32 | 3.877 | 139.16 | 21,575.84 | 3.185 | 14.19 | 847.73 | 81.063 | 8.80 |

## HDF5 Direct I/O Write Benchmark, stripe comparisons, memory-aligned host-buffer, 2 nodes 16GCDs:

| HDF5 access mode | `H5FD_MPIO_INDEPENDENT` |
| MPI-IO hint | `"direct_io"` |
| HDF5 alignment | `H5Pset_alignment(4194304, 4194304)` |

> Each tested size is **per MPI rank**. With 16 ranks, the aggregate file size is 16× the listed size.

### 2 OSTs × 1 MiB

- Lustre layout: `stripe_count=2`, `stripe_size=1 MiB`, `pattern=raid0`, `stripe_offset=-1`

| Size/rank | Allocation | D2H mean (ms) | D2H bandwidth (GB/s) | Direct I/O mean (ms) | Direct I/O bandwidth (GB/s) | Std. dev. (ms) | CV (%) | Median (ms) |
|---:|---|---:|---:|---:|---:|---:|---:|---:|
| 1 GB | Pageable | 47.78 | 22.47 | 6268.74 | 2.74 | 389.45 | 6.2 | 6469.20 |
| 1 GB | Pinned | 51.12 | 21.00 | 2476.04 | 6.94 | 59.02 | 2.4 | 2485.21 |
| 1 GB | Managed | 47.65 | 22.53 | 2441.95 | 7.04 | 62.55 | 2.6 | 2446.00 |
| 2 GB | Pageable | 95.68 | 22.45 | 12568.31 | 2.73 | 1548.74 | 12.3 | 11943.76 |
| 2 GB | Pinned | 96.95 | 22.15 | 12244.97 | 2.81 | 1382.60 | 11.3 | 11840.05 |
| 2 GB | Managed | 96.41 | 22.27 | 12311.09 | 2.79 | 1522.80 | 12.4 | 11618.86 |
| 3 GB | Pageable | 143.71 | 22.41 | 19654.21 | 2.62 | 2136.73 | 10.9 | 19889.11 |
| 3 GB | Pinned | 146.93 | 21.92 | 19440.72 | 2.65 | 2325.78 | 12.0 | 19834.49 |
| 3 GB | Managed | 142.88 | 22.54 | 19786.77 | 2.60 | 2537.20 | 12.8 | 19864.76 |
| 4 GB | Pageable | 191.30 | 22.45 | 26933.06 | 2.55 | 3081.95 | 11.4 | 28185.18 |
| 4 GB | Pinned | 198.01 | 21.69 | 26999.56 | 2.55 | 3261.47 | 12.1 | 28852.46 |
| 4 GB | Managed | 195.50 | 21.97 | 26704.41 | 2.57 | 3364.00 | 12.6 | 28398.35 |
| 8 GB | Pageable | 382.63 | 22.45 | 55670.38 | 2.47 | 5226.81 | 9.4 | 58052.55 |
| 8 GB | Pinned | 402.38 | 21.35 | 55186.92 | 2.49 | 5293.23 | 9.6 | 57636.53 |
| 8 GB | Managed | 404.36 | 21.24 | 56374.41 | 2.44 | 5268.14 | 9.3 | 58889.45 |

### 8 OSTs × 1 MiB

- Lustre layout: `stripe_count=8`, `stripe_size=1 MiB`, `pattern=raid0`, `stripe_offset=-1`

| Size/rank | Allocation | D2H mean (ms) | D2H bandwidth (GB/s) | Direct I/O mean (ms) | Direct I/O bandwidth (GB/s) | Std. dev. (ms) | CV (%) | Median (ms) |
|---:|---|---:|---:|---:|---:|---:|---:|---:|
| 1 GB | Pageable | 47.82 | 22.45 | 6568.14 | 2.62 | 478.94 | 7.3 | 6804.75 |
| 1 GB | Pinned | 47.63 | 22.54 | 2498.42 | 6.88 | 106.31 | 4.3 | 2509.39 |
| 1 GB | Managed | 47.65 | 22.54 | 2471.01 | 6.95 | 109.05 | 4.4 | 2505.18 |
| 2 GB | Pageable | 95.71 | 22.44 | 13702.04 | 2.51 | 1279.58 | 9.3 | 14016.91 |
| 2 GB | Pinned | 99.50 | 21.58 | 13854.42 | 2.48 | 1271.51 | 9.2 | 14185.49 |
| 2 GB | Managed | 95.92 | 22.39 | 12999.29 | 2.64 | 1195.36 | 9.2 | 12548.56 |
| 3 GB | Pageable | 143.53 | 22.44 | 20506.48 | 2.51 | 2049.02 | 10.0 | 20948.06 |
| 3 GB | Pinned | 146.77 | 21.95 | 19709.91 | 2.61 | 1678.52 | 8.5 | 20662.03 |
| 3 GB | Managed | 152.19 | 21.17 | 20853.94 | 2.47 | 2002.37 | 9.6 | 21177.28 |
| 4 GB | Pageable | 247.07 | 17.38 | 27915.24 | 2.46 | 2819.05 | 10.1 | 27458.57 |
| 4 GB | Pinned | 193.98 | 22.14 | 27635.19 | 2.49 | 2811.03 | 10.2 | 28175.18 |
| 4 GB | Managed | 197.43 | 21.75 | 28043.82 | 2.45 | 2521.86 | 9.0 | 28043.19 |
| 8 GB | Pageable | 383.03 | 22.43 | 56332.91 | 2.44 | 3854.37 | 6.8 | 56739.68 |
| 8 GB | Pinned | 436.21 | 19.69 | 56214.93 | 2.44 | 3593.79 | 6.4 | 56937.25 |
| 8 GB | Managed | 394.09 | 21.80 | 57201.98 | 2.40 | 3891.98 | 6.8 | 58252.46 |

### 8 OSTs × 4 MiB

- Lustre layout: `stripe_count=8`, `stripe_size=4 MiB`, `pattern=raid0`, `stripe_offset=-1`

| Size/rank | Allocation | D2H mean (ms) | D2H bandwidth (GB/s) | Direct I/O mean (ms) | Direct I/O bandwidth (GB/s) | Std. dev. (ms) | CV (%) | Median (ms) |
|---:|---|---:|---:|---:|---:|---:|---:|---:|
| 1 GB | Pageable | 47.81 | 22.46 | 6459.98 | 2.66 | 507.42 | 7.9 | 6659.69 |
| 1 GB | Pinned | 47.84 | 22.44 | 3379.52 | 5.08 | 229.16 | 6.8 | 3413.96 |
| 1 GB | Managed | 50.58 | 21.23 | 3327.36 | 5.16 | 226.05 | 6.8 | 3279.87 |
| 2 GB | Pageable | 284.49 | 7.55 | 12374.85 | 2.78 | 1125.44 | 9.1 | 11946.83 |
| 2 GB | Pinned | 95.29 | 22.54 | 12957.54 | 2.65 | 951.52 | 7.3 | 12986.36 |
| 2 GB | Managed | 98.46 | 21.81 | 13081.35 | 2.63 | 967.84 | 7.4 | 13545.97 |
| 3 GB | Pageable | 158.23 | 20.36 | 19921.79 | 2.59 | 2647.72 | 13.3 | 19588.93 |
| 3 GB | Pinned | 144.88 | 22.23 | 19720.38 | 2.61 | 2664.77 | 13.5 | 19286.29 |
| 3 GB | Managed | 144.52 | 22.29 | 19946.43 | 2.58 | 2380.04 | 11.9 | 19663.19 |
| 4 GB | Pageable | 191.51 | 22.43 | 27294.47 | 2.52 | 3129.27 | 11.5 | 26997.22 |
| 4 GB | Pinned | 194.42 | 22.09 | 28010.77 | 2.45 | 2925.86 | 10.4 | 29153.93 |
| 4 GB | Managed | 208.57 | 20.59 | 27162.00 | 2.53 | 2997.19 | 11.0 | 26743.33 |
| 8 GB | Pageable | 673.39 | 12.76 | 56542.44 | 2.43 | 4616.08 | 8.2 | 58574.31 |
| 8 GB | Pinned | 381.11 | 22.54 | 57166.96 | 2.40 | 4451.09 | 7.8 | 59241.45 |
| 8 GB | Managed | 393.69 | 21.82 | 57093.44 | 2.41 | 4644.57 | 8.1 | 59061.27 |

### 16 OSTs × 1 MiB

- Lustre layout: `stripe_count=16`, `stripe_size=1 MiB`, `pattern=raid0`, `stripe_offset=-1`

| Size/rank | Allocation | D2H mean (ms) | D2H bandwidth (GB/s) | Direct I/O mean (ms) | Direct I/O bandwidth (GB/s) | Std. dev. (ms) | CV (%) | Median (ms) |
|---:|---|---:|---:|---:|---:|---:|---:|---:|
| 1 GB | Pageable | 47.80 | 22.46 | 7047.56 | 2.44 | 465.79 | 6.6 | 7165.02 |
| 1 GB | Pinned | 48.24 | 22.26 | 4118.08 | 4.17 | 192.11 | 4.7 | 4175.94 |
| 1 GB | Managed | 48.48 | 22.15 | 4041.99 | 4.25 | 254.99 | 6.3 | 4075.05 |
| 2 GB | Pageable | 95.58 | 22.47 | 14582.70 | 2.36 | 1504.21 | 10.3 | 14620.32 |
| 2 GB | Pinned | 96.22 | 22.32 | 14372.09 | 2.39 | 1379.45 | 9.6 | 14311.56 |
| 2 GB | Managed | 102.08 | 21.04 | 13991.79 | 2.46 | 1334.09 | 9.5 | 13617.05 |
| 3 GB | Pageable | 159.81 | 20.16 | 21124.74 | 2.44 | 1843.42 | 8.7 | 21499.36 |
| 3 GB | Pinned | 142.90 | 22.54 | 20605.29 | 2.50 | 1638.56 | 8.0 | 21037.74 |
| 3 GB | Managed | 156.72 | 20.55 | 21427.00 | 2.41 | 1919.64 | 9.0 | 21726.49 |
| 4 GB | Pageable | 191.27 | 22.45 | 28993.13 | 2.37 | 2795.03 | 9.6 | 29516.27 |
| 4 GB | Pinned | 190.58 | 22.54 | 27612.04 | 2.49 | 2887.84 | 10.5 | 27398.52 |
| 4 GB | Managed | 194.32 | 22.10 | 29306.04 | 2.34 | 2870.07 | 9.8 | 29299.45 |
| 8 GB | Pageable | 422.39 | 20.34 | 57257.70 | 2.40 | 3490.87 | 6.1 | 57851.04 |
| 8 GB | Pinned | 394.95 | 21.75 | 57600.09 | 2.39 | 3224.81 | 5.6 | 58347.05 |
| 8 GB | Managed | 398.27 | 21.57 | 57728.29 | 2.38 | 3428.57 | 5.9 | 58258.35 |

### 16 OSTs × 4 MiB

- Lustre layout: `stripe_count=16`, `stripe_size=4 MiB`, `pattern=raid0`, `stripe_offset=-1`

| Size/rank | Allocation | D2H mean (ms) | D2H bandwidth (GB/s) | Direct I/O mean (ms) | Direct I/O bandwidth (GB/s) | Std. dev. (ms) | CV (%) | Median (ms) |
|---:|---|---:|---:|---:|---:|---:|---:|---:|
| 1 GB | Pageable | 47.79 | 22.47 | 6807.66 | 2.52 | 495.21 | 7.3 | 6565.61 |
| 1 GB | Pinned | 49.08 | 21.88 | 6927.82 | 2.48 | 486.98 | 7.0 | 6722.58 |
| 1 GB | Managed | 51.82 | 20.72 | 6732.25 | 2.55 | 610.32 | 9.1 | 6590.34 |
| 2 GB | Pageable | 149.42 | 14.37 | 14048.12 | 2.45 | 1222.61 | 8.7 | 14105.56 |
| 2 GB | Pinned | 99.08 | 21.67 | 14078.81 | 2.44 | 1232.17 | 8.8 | 14360.58 |
| 2 GB | Managed | 96.90 | 22.16 | 13963.44 | 2.46 | 1665.02 | 11.9 | 13781.48 |
| 3 GB | Pageable | 143.49 | 22.45 | 20822.37 | 2.48 | 1760.67 | 8.5 | 21579.35 |
| 3 GB | Pinned | 142.97 | 22.53 | 20405.82 | 2.53 | 2199.35 | 10.8 | 20046.96 |
| 3 GB | Managed | 149.48 | 21.55 | 21415.40 | 2.41 | 2309.23 | 10.8 | 21936.77 |
| 4 GB | Pageable | 191.32 | 22.45 | 29422.27 | 2.34 | 2803.38 | 9.5 | 30495.41 |
| 4 GB | Pinned | 192.08 | 22.36 | 28741.84 | 2.39 | 3007.03 | 10.5 | 28731.64 |
| 4 GB | Managed | 190.63 | 22.53 | 29201.55 | 2.35 | 2666.42 | 9.1 | 30082.95 |
| 8 GB | Pageable | 382.79 | 22.44 | 55919.73 | 2.46 | 3992.39 | 7.1 | 56770.41 |
| 8 GB | Pinned | 390.04 | 22.02 | 56851.62 | 2.42 | 3834.28 | 6.7 | 57742.36 |
| 8 GB | Managed | 395.13 | 21.74 | 57092.14 | 2.41 | 3895.09 | 6.8 | 57684.79 |

### 24 OSTs × 1 MiB

- Lustre layout: `stripe_count=24`, `stripe_size=1 MiB`, `pattern=raid0`, `stripe_offset=-1`

| Size/rank | Allocation | D2H mean (ms) | D2H bandwidth (GB/s) | Direct I/O mean (ms) | Direct I/O bandwidth (GB/s) | Std. dev. (ms) | CV (%) | Median (ms) |
|---:|---|---:|---:|---:|---:|---:|---:|---:|
| 1 GB | Pageable | 47.84 | 22.44 | 7211.04 | 2.38 | 359.65 | 5.0 | 7264.82 |
| 1 GB | Pinned | 47.72 | 22.50 | 3985.16 | 4.31 | 236.05 | 5.9 | 3989.94 |
| 1 GB | Managed | 48.57 | 22.11 | 4055.92 | 4.24 | 189.08 | 4.7 | 4017.57 |
| 2 GB | Pageable | 95.69 | 22.44 | 14722.67 | 2.33 | 1755.71 | 11.9 | 14200.62 |
| 2 GB | Pinned | 95.83 | 22.41 | 14780.79 | 2.32 | 1843.95 | 12.5 | 14886.39 |
| 2 GB | Managed | 95.93 | 22.39 | 14486.53 | 2.37 | 1605.34 | 11.1 | 13719.60 |
| 3 GB | Pageable | 174.75 | 18.43 | 22460.03 | 2.29 | 2415.22 | 10.8 | 22583.16 |
| 3 GB | Pinned | 145.76 | 22.10 | 22013.82 | 2.34 | 2325.14 | 10.6 | 21569.28 |
| 3 GB | Managed | 147.08 | 21.90 | 22876.41 | 2.25 | 2071.31 | 9.1 | 23481.49 |
| 4 GB | Pageable | 191.38 | 22.44 | 29610.02 | 2.32 | 2894.31 | 9.8 | 30081.02 |
| 4 GB | Pinned | 190.72 | 22.52 | 30270.54 | 2.27 | 3638.21 | 12.0 | 30336.77 |
| 4 GB | Managed | 199.20 | 21.56 | 30420.08 | 2.26 | 2987.59 | 9.8 | 30075.99 |
| 8 GB | Pageable | 383.12 | 22.42 | 59807.08 | 2.30 | 3778.47 | 6.3 | 60585.50 |
| 8 GB | Pinned | 394.88 | 21.75 | 60212.52 | 2.28 | 3622.20 | 6.0 | 61277.60 |
| 8 GB | Managed | 400.74 | 21.44 | 58723.46 | 2.34 | 3424.40 | 5.8 | 59529.67 |

### 24 OSTs × 4 MiB

- Lustre layout: `stripe_count=24`, `stripe_size=4 MiB`, `pattern=raid0`, `stripe_offset=-1`

| Size/rank | Allocation | D2H mean (ms) | D2H bandwidth (GB/s) | Direct I/O mean (ms) | Direct I/O bandwidth (GB/s) | Std. dev. (ms) | CV (%) | Median (ms) |
|---:|---|---:|---:|---:|---:|---:|---:|---:|
| 1 GB | Pageable | 47.80 | 22.47 | 6910.22 | 2.49 | 359.69 | 5.2 | 7006.31 |
| 1 GB | Pinned | 47.65 | 22.54 | 6728.02 | 2.55 | 295.53 | 4.4 | 6722.25 |
| 1 GB | Managed | 47.67 | 22.52 | 7077.30 | 2.43 | 381.98 | 5.4 | 7195.04 |
| 2 GB | Pageable | 95.69 | 22.44 | 13925.98 | 2.47 | 1751.45 | 12.6 | 13296.90 |
| 2 GB | Pinned | 95.41 | 22.51 | 13878.74 | 2.48 | 1776.19 | 12.8 | 13443.70 |
| 2 GB | Managed | 106.14 | 20.23 | 14025.36 | 2.45 | 1617.28 | 11.5 | 13596.74 |
| 3 GB | Pageable | 422.74 | 7.62 | 21613.18 | 2.38 | 2578.54 | 11.9 | 21627.52 |
| 3 GB | Pinned | 146.37 | 22.01 | 21976.67 | 2.35 | 2316.13 | 10.5 | 22888.47 |
| 3 GB | Managed | 147.69 | 21.81 | 21385.65 | 2.41 | 3001.82 | 14.0 | 21049.45 |
| 4 GB | Pageable | 191.37 | 22.44 | 29220.66 | 2.35 | 3247.06 | 11.1 | 29594.38 |
| 4 GB | Pinned | 190.62 | 22.53 | 29586.47 | 2.32 | 3978.04 | 13.4 | 31009.78 |
| 4 GB | Managed | 193.89 | 22.15 | 28792.56 | 2.39 | 3694.43 | 12.8 | 29293.29 |
| 8 GB | Pageable | 382.74 | 22.44 | 57956.08 | 2.37 | 4512.39 | 7.8 | 58803.16 |
| 8 GB | Pinned | 387.53 | 22.17 | 58581.36 | 2.35 | 4250.02 | 7.3 | 59448.26 |
| 8 GB | Managed | 398.61 | 21.55 | 56920.04 | 2.41 | 4299.82 | 7.6 | 57787.54 |

## Scalability: HDF5 individual & direct I/O on 1 -> 16 GPUs, default striping (2 x 1MiB)

### SSD writes:

| GPUs / MPI ranks | Data per rank | Total data | Pageable time (ms) | Pageable (GB/s) | Pinned time (ms) | Pinned (GB/s) | Managed time (ms) | Managed (GB/s) |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 | 1 GB | 1 GB | 1,086.91 | 0.99 | 1,093.07 | 0.98 | 1,095.03 | 0.98 |
| 1 | 2 GB | 2 GB | 2,201.73 | 0.98 | 2,172.27 | 0.99 | 2,183.71 | 0.98 |
| 1 | 3 GB | 3 GB | 3,309.97 | 0.97 | 3,240.91 | 0.99 | 3,230.36 | 1.00 |
| 1 | 4 GB | 4 GB | 4,450.04 | 0.97 | 4,381.17 | 0.98 | 4,346.54 | 0.99 |
| 1 | 8 GB | 8 GB | 9,099.60 | 0.94 | 8,982.04 | 0.96 | 8,947.42 | 0.96 |
| 2 | 1 GB | 2 GB | 1,547.15 | 1.39 | 1,128.63 | 1.90 | 1,159.23 | 1.85 |
| 2 | 2 GB | 4 GB | 3,018.48 | 1.42 | 3,075.13 | 1.40 | 3,073.52 | 1.40 |
| 2 | 3 GB | 6 GB | 4,839.60 | 1.33 | 4,855.31 | 1.33 | 4,809.55 | 1.34 |
| 2 | 4 GB | 8 GB | 6,423.17 | 1.34 | 6,667.77 | 1.29 | 6,636.83 | 1.29 |
| 2 | 8 GB | 16 GB | 13,335.10 | 1.29 | 12,636.08 | 1.36 | 12,785.64 | 1.34 |
| 4 | 1 GB | 4 GB | 2,709.66 | 1.59 | 1,331.44 | 3.23 | 1,315.08 | 3.27 |
| 4 | 2 GB | 8 GB | 5,472.58 | 1.57 | 5,415.82 | 1.59 | 5,621.76 | 1.53 |
| 4 | 3 GB | 12 GB | 8,847.73 | 1.46 | 8,764.05 | 1.47 | 8,719.48 | 1.48 |
| 4 | 4 GB | 16 GB | 12,227.57 | 1.41 | 11,901.96 | 1.44 | 12,468.35 | 1.38 |
| 4 | 8 GB | 32 GB | 26,605.86 | 1.29 | 25,110.32 | 1.37 | 25,225.00 | 1.36 |
| 8 | 1 GB | 8 GB | 5,469.39 | 1.57 | 1,836.50 | 4.68 | 1,916.57 | 4.48 |
| 8 | 2 GB | 16 GB | 11,709.26 | 1.47 | 11,405.29 | 1.51 | 11,853.26 | 1.45 |
| 8 | 3 GB | 24 GB | 18,886.57 | 1.36 | 19,109.79 | 1.35 | 19,225.42 | 1.34 |
| 8 | 4 GB | 32 GB | 25,957.10 | 1.32 | 26,750.32 | 1.28 | 26,208.79 | 1.31 |
| 8 | 8 GB | 64 GB | 56,138.32 | 1.22 | 54,684.11 | 1.26 | 55,457.09 | 1.24 |
| 16† | 1 GB | 16 GB | 6,268.74 | 2.74 | 2,476.04 | 6.94 | 2,441.95 | 7.04 |
| 16† | 2 GB | 32 GB | 12,568.31 | 2.73 | 12,244.97 | 2.81 | 12,311.09 | 2.79 |
| 16† | 3 GB | 48 GB | 19,654.21 | 2.62 | 19,440.72 | 2.65 | 19,786.77 | 2.60 |
| 16† | 4 GB | 64 GB | 26,933.06 | 2.55 | 26,999.56 | 2.55 | 26,704.41 | 2.57 |
| 16† | 8 GB | 128 GB | 55,670.38 | 2.47 | 55,186.92 | 2.49 | 56,374.41 | 2.44 |

### D2H copy times (device → pinned/pageable/managed host buffer), 1 -> 8 GPUs (time-limit killed job at 16 GPUs)

| GPUs | Data/rank | Pageable D2H (ms) | Pageable D2H (GB/s) | Pinned D2H (ms) | Pinned D2H (GB/s) | Managed D2H (ms) | Managed D2H (GB/s) |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 | 1 GB | 40.54 | 26.48 | 40.50 | 26.51 | 40.50 | 26.51 |
| 1 | 2 GB | 81.08 | 26.49 | 80.99 | 26.52 | 80.99 | 26.52 |
| 1 | 3 GB | 121.59 | 26.49 | 121.47 | 26.52 | 121.47 | 26.52 |
| 1 | 4 GB | 162.13 | 26.49 | 161.97 | 26.52 | 161.96 | 26.52 |
| 1 | 8 GB | 324.24 | 26.49 | 323.94 | 26.52 | 323.93 | 26.52 |
| 2 | 1 GB | 47.82 | 22.45 | 47.67 | 22.52 | 47.70 | 22.51 |
| 2 | 2 GB | 95.57 | 22.47 | 95.31 | 22.53 | 95.35 | 22.52 |
| 2 | 3 GB | 143.91 | 22.38 | 143.00 | 22.53 | 143.05 | 22.52 |
| 2 | 4 GB | 191.28 | 22.45 | 190.64 | 22.53 | 190.63 | 22.53 |
| 2 | 8 GB | 382.60 | 22.45 | 381.18 | 22.53 | 381.30 | 22.53 |
| 4 | 1 GB | 47.86 | 22.44 | 47.67 | 22.53 | 47.82 | 22.45 |
| 4 | 2 GB | 95.71 | 22.44 | 95.30 | 22.53 | 95.33 | 22.53 |
| 4 | 3 GB | 143.53 | 22.44 | 142.83 | 22.55 | 143.44 | 22.46 |
| 4 | 4 GB | 191.24 | 22.46 | 190.58 | 22.54 | 190.66 | 22.53 |
| 4 | 8 GB | 382.49 | 22.46 | 381.18 | 22.53 | 381.65 | 22.51 |
| 8 | 1 GB | 47.87 | 22.43 | 47.70 | 22.51 | 47.70 | 22.51 |
| 8 | 2 GB | 95.68 | 22.45 | 114.70† | 18.72† | 95.33 | 22.53 |
| 8 | 3 GB | 143.57 | 22.44 | 143.23 | 22.49 | 143.43 | 22.46 |
| 8 | 4 GB | 191.37 | 22.44 | 190.57 | 22.54 | 190.63 | 22.53 |
| 8 | 8 GB | 382.89 | 22.43 | 387.51 | 22.17 | 396.95 | 21.64 |

## Scalability: MPI collective & direct I/O on 1 -> 16 GPUs, default striping (2 x 1MiB)

| GPUs / MPI ranks | Size per rank | Total data | D2H mean (ms) | D2H (GB/s) | Collective mean (ms) | Collective (GB/s) | Collective std. (ms) | Collective CV (%) | Collective median (ms) | Direct I/O mean (ms) | Direct I/O (GB/s) | Direct I/O std. (ms) | Direct I/O CV (%) | Direct I/O median (ms) |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 | 1 GB | 1 GB | 40.50 | 26.51 | 837.92 | 1.28 | 8.04 | 1.0 | 837.85 | 173.93 | 6.17 | 4.26 | 2.5 | 172.64 |
| 1 | 2 GB | 2 GB | 80.99 | 26.52 | 2,401.76 | 0.89 | 1,861.61 | 77.5 | 1,686.25 | 356.65 | 6.02 | 2.20 | 0.6 | 356.31 |
| 1 | 4 GB | 4 GB | 161.96 | 26.52 | 3,816.63 | 1.13 | 895.05 | 23.5 | 3,413.79 | 1,336.54 | 3.21 | 425.38 | 31.8 | 1,357.41 |
| 1 | 8 GB | 8 GB | 323.94 | 26.52 | 18,325.91 | 0.47 | 798.62 | 4.4 | 18,559.02 | 1,691.17 | 5.08 | 318.68 | 18.8 | 1,699.66 |
| 2 | 1 GB | 2 GB | 47.66 | 22.53 | 3,339.53 | 0.64 | 1,762.38 | 52.8 | 3,774.81 | 200.92 | 10.69 | 2.80 | 1.4 | 201.78 |
| 2 | 2 GB | 4 GB | 95.26 | 22.54 | 2,931.09 | 1.47 | 950.14 | 32.4 | 2,478.97 | 1,467.33 | 2.93 | 199.98 | 13.6 | 1,419.53 |
| 2 | 4 GB | 8 GB | 190.59 | 22.53 | 14,397.77 | 0.60 | 1,114.89 | 7.7 | 14,696.42 | 2,336.27 | 3.68 | 369.70 | 15.8 | 2,199.90 |
| 2 | 8 GB | 16 GB | 381.55 | 22.51 | 26,763.71 | 0.64 | 3,677.96 | 13.7 | 27,729.32 | 4,546.88 | 3.78 | 1,387.97 | 30.5 | 4,638.97 |
| 4 | 1 GB | 4 GB | 47.70 | 22.51 | 6,786.24 | 0.63 | 1,938.64 | 28.6 | 7,809.42 | 3,003.49 | 1.43 | 3,191.63 | 106.3 | 1,402.73 |
| 4 | 2 GB | 8 GB | 95.45 | 22.50 | 12,931.51 | 0.66 | 1,353.25 | 10.5 | 12,753.10 | 13,112.19 | 0.66 | 6,591.24 | 50.3 | 12,938.32 |
| 4 | 4 GB | 16 GB | 190.53 | 22.54 | 26,146.61 | 0.66 | 1,551.51 | 5.9 | 26,143.71 | 16,095.44 | 1.07 | 2,955.46 | 18.4 | 16,081.91 |
| 4 | 8 GB | 32 GB | 381.32 | 22.53 | 44,906.64 | 0.77 | 9,613.76 | 21.4 | 45,945.05 | 6,589.55 | 5.21 | 508.53 | 7.7 | 6,460.05 |
| 8 | 1 GB | 8 GB | 47.68 | 22.52 | 8,365.29 | 1.03 | 1,721.52 | 20.6 | 9,156.31 | 5,874.97 | 1.46 | 9,432.95 | 160.6 | 1,588.30 |
| 8 | 2 GB | 16 GB | 95.31 | 22.53 | 18,329.80 | 0.94 | 1,238.57 | 6.8 | 18,637.16 | 7,663.13 | 2.24 | 9,005.27 | 117.5 | 3,651.00 |
| 8 | 4 GB | 32 GB | 190.60 | 22.53 | 31,551.78 | 1.09 | 7,517.23 | 23.8 | 34,604.87 | 17,896.46 | 1.92 | 8,659.86 | 48.4 | 18,678.73 |
| 8 | 8 GB | 64 GB | 382.98 | 22.43 | 57,991.42 | 1.18 | 13,801.14 | 23.8 | 66,397.23 | 19,687.84 | 3.49 | 2,987.75 | 15.2 | 19,927.82 |
| 16 | 1 GB | 16 GB | 47.68 | 22.52 | 23,720.24 | 0.72 | 925.48 | 3.9 | 23,948.41 | 10,068.87 | 1.71 | 4,716.22 | 46.8 | 10,336.02 |
| 16 | 2 GB | 32 GB | 96.54 | 22.25 | 32,306.41 | 1.06 | 11,029.95 | 34.1 | 36,705.20 | 4,501.49 | 7.63 | 121.77 | 2.7 | 4,492.52 |
| 16 | 4 GB | 64 GB | 191.68 | 22.41 | 29,569.84 | 2.32 | 5,527.29 | 18.7 | 27,601.41 | 10,516.52 | 6.53 | 2,387.32 | 22.7 | 9,472.53 |
| 16 | 8 GB | 128 GB | 384.16 | 22.36 | 91,130.48 | 1.51 | 1,084.76 | 1.2 | 91,013.62 | 18,825.56 | 7.30 | 983.93 | 5.2 | 18,473.80 |

## Scalability: testing 4 nodes (32 ranks), Lustre default striping

### MPI collective and direct I/O

| Size per GPU | Allocation | D2H mean (ms) | D2H bandwidth (GB/s) | Strategy   | Write mean (ms) | Write bandwidth (GB/s) | Std. dev. (ms) | CV (%) | Median (ms) |
| -----------: | ---------- | ------------: | -------------------: | ---------- | --------------: | ---------------------: | -------------: | -----: | ----------: |
|         1 GB | Pinned     |         47.70 |                22.51 | Collective |       25,237.69 |                   1.36 |       4,916.40 |   19.5 |   26,381.09 |
|         1 GB | Pinned     |         47.70 |                22.51 | Direct I/O |        5,614.39 |                   6.12 |       1,092.80 |   19.5 |    5,165.28 |
|         2 GB | Pinned     |         95.23 |                22.55 | Collective |       42,813.30 |                   1.61 |      13,846.67 |   32.3 |   49,764.43 |
|         2 GB | Pinned     |         95.23 |                22.55 | Direct I/O |        8,685.20 |                   7.91 |       3,285.61 |   37.8 |    6,455.34 |
|         4 GB | Pinned     |        191.70 |                22.41 | Collective |       97,346.29 |                   1.41 |       3,407.50 |    3.5 |   96,531.45 |
|         4 GB | Pinned     |        191.70 |                22.41 | Direct I/O |       12,047.62 |                  11.41 |         883.13 |    7.3 |   12,202.29 |
|         8 GB | Pinned     |        398.58 |                21.55 | Collective |      189,701.52 |                   1.45 |       2,393.99 |    1.3 |  189,529.48 |
|         8 GB | Pinned     |        398.58 |                21.55 | Direct I/O |       24,298.68 |                  11.31 |       1,531.88 |    6.3 |   24,309.20 |


### HDF5 collective and independent without direct I/O

| Data size | Allocation | D2H time (ms) | D2H bandwidth (GB/s) | Strategy    | Write time (ms) | Write bandwidth (GB/s) |       Std dev | Std dev (%) | Median (ms) |
| --------: | ---------- | ------------: | -------------------: | ----------- | --------------: | ---------------------: | ------------: | ----------: | ----------: |
|      1 GB | Pinned     |         47.69 |                22.52 | Independent |        5,750.21 |                   5.98 |     242.71 ms |        4.2% |    5,648.00 |
|      1 GB | Pinned     |         47.69 |                22.52 | Collective  |       20,841.02 |                   1.65 |     837.58 ms |        4.0% |   21,271.39 |
|      2 GB | Pinned     |         95.35 |                22.52 | Independent |       11,927.41 |                   5.76 |   1,060.16 ms |        8.9% |   11,913.04 |
|      2 GB | Pinned     |         95.35 |                22.52 | Collective  |       45,542.82 |                   1.51 |   2,379.23 ms |        5.2% |   46,187.96 |
|      4 GB | Pinned     |        197.21 |                21.78 | Independent |       27,353.72 |                   5.02 |   2,979.28 ms |       10.9% |   29,565.13 |
|      4 GB | Pinned     |        197.21 |                21.78 | Collective  |      101,584.38 |                   1.35 |  13,197.09 ms |       13.0% |  100,218.72 |
|      8 GB | Pinned     |        393.31 |                21.84 | Independent |       56,734.28 |                   4.85 |   3,682.36 ms |        6.5% |   58,618.20 |
|      8 GB | Pinned     |        393.31 |                21.84 | Collective  |      297,288.41 |                   0.92 | 118,744.80 ms |       39.9% |  257,824.52 |



  ### HDF5 direct I/O :

  | Data size / rank | Total data (32 ranks) | D2H time (ms) | D2H bandwidth (GB/s) | Direct I/O time (ms) | Direct I/O bandwidth (GB/s) | Std. dev. (ms) | Std. dev. (%) | Median (ms) |
| ---------------: | --------------------: | ------------: | -------------------: | -------------------: | --------------------------: | -------------: | ------------: | ----------: |
|             1 GB |                 32 GB |         47.91 |                22.41 |             3,704.75 |                        9.27 |         110.19 |          3.0% |    3,674.70 |
|             2 GB |                 64 GB |         96.25 |                22.31 |             6,384.52 |                       10.76 |         152.07 |          2.4% |    6,328.14 |
|             4 GB |                128 GB |        190.57 |                22.54 |            11,959.55 |                       11.49 |         493.65 |          4.1% |   12,041.16 |
|             8 GB |                256 GB |        398.42 |                21.56 |            24,770.29 |                       11.10 |       1,241.52 |          5.0% |   24,840.53 |


### HDF5 direct I/O on 16 ranks, different stripe configurations : 

| Stripe config  | Data size | D2H (ms) | D2H (GB/s) | Direct I/O avg (ms) | Direct I/O (GB/s) |            Std dev | Median (ms) |
| -------------- | --------: | -------: | ---------: | ------------------: | ----------------: | -----------------: | ----------: |
| **2 × 1 MiB**  |      1 MB |     0.06 |      18.98 |               20.44 |              0.82 |    3.29 ms (16.1%) |       21.56 |
|                |     64 MB |     3.00 |      22.39 |              344.36 |              3.12 |  119.16 ms (34.6%) |      351.10 |
|                |    256 MB |    11.94 |      22.49 |              716.24 |          **6.00** |     9.67 ms (1.3%) |      715.51 |
|                |      1 GB |    47.69 |      22.52 |             2450.17 |          **7.01** |   124.60 ms (5.1%) |     2422.55 |
|                |      2 GB |    95.29 |      22.54 |             4577.58 |          **7.51** |   317.85 ms (6.9%) |     4493.45 |
|                |      4 GB |   192.92 |      22.26 |             8514.81 |          **8.07** |   464.34 ms (5.5%) |     8469.38 |
|                |      8 GB |   395.45 |      21.72 |            16696.68 |          **8.23** |  1173.40 ms (7.0%) |    16649.74 |
| **8 × 1 MiB**  |      1 MB |     0.06 |      18.35 |               15.18 |              1.11 |    4.22 ms (27.8%) |       13.82 |
|                |     64 MB |     2.99 |      22.43 |              273.05 |              3.93 |  226.92 ms (83.1%) |      207.92 |
|                |    256 MB |    11.93 |      22.49 |              740.49 |              5.80 |    62.54 ms (8.4%) |      747.78 |
|                |      1 GB |    47.69 |      22.52 |             2472.55 |              6.95 |    85.82 ms (3.5%) |     2450.19 |
|                |      2 GB |    95.76 |      22.43 |             4596.25 |              7.48 |   157.86 ms (3.4%) |     4577.88 |
|                |      4 GB |   198.33 |      21.66 |             8880.81 |              7.74 |   242.36 ms (2.7%) |     8814.61 |
|                |      8 GB |   386.35 |      22.23 |            18222.09 |              7.54 |   909.85 ms (5.0%) |    17855.82 |
| **8 × 4 MiB**  |      1 MB |     0.06 |      18.29 |               14.85 |              1.13 |    1.89 ms (12.8%) |       14.69 |
|                |     64 MB |     2.99 |      22.42 |              223.91 |          **4.80** |    13.24 ms (5.9%) |      221.39 |
|                |    256 MB |    11.93 |      22.51 |              803.52 |              5.35 |    50.00 ms (6.2%) |      781.02 |
|                |      1 GB |    47.66 |      22.53 |             3071.60 |              5.59 |    37.78 ms (1.2%) |     3085.03 |
|                |      2 GB |   102.65 |      20.92 |             6035.55 |              5.69 |    76.61 ms (1.3%) |     6016.01 |
|                |      4 GB |   196.37 |      21.87 |            13030.75 |              5.27 |  1103.66 ms (8.5%) |    12484.37 |
|                |      8 GB |   411.85 |      20.86 |            27789.77 |              4.95 |  2015.31 ms (7.3%) |    28876.86 |
| **16 × 1 MiB** |      1 MB |     0.06 |      17.96 |               11.31 |          **1.48** |    2.11 ms (18.6%) |       11.46 |
|                |     64 MB |     3.00 |      22.36 |              271.11 |              3.96 |    18.36 ms (6.8%) |      266.31 |
|                |    256 MB |    11.93 |      22.49 |             1015.40 |              4.23 |    72.93 ms (7.2%) |     1013.13 |
|                |      1 GB |    49.00 |      21.91 |             3897.48 |              4.41 |   279.12 ms (7.2%) |     3836.75 |
|                |      2 GB |    95.80 |      22.42 |             7240.60 |              4.75 |   412.68 ms (5.7%) |     7313.43 |
|                |      4 GB |   190.63 |      22.53 |            14240.76 |              4.83 |  1223.95 ms (8.6%) |    13945.03 |
|                |      8 GB |   391.66 |      21.93 |            30403.77 |              4.52 |  2570.64 ms (8.5%) |    30565.62 |
| **16 × 4 MiB** |      1 MB |     0.06 |      18.75 |               14.98 |              1.12 |    2.62 ms (17.5%) |       15.33 |
|                |     64 MB |     3.00 |      22.37 |              564.09 |              1.90 |  121.84 ms (21.6%) |      503.67 |
|                |    256 MB |    12.10 |      22.19 |             1688.42 |              2.54 |  425.43 ms (25.2%) |     1444.70 |
|                |      1 GB |    50.72 |      21.17 |             6159.81 |              2.79 |  803.67 ms (13.0%) |     5751.65 |
|                |      2 GB |    96.71 |      22.20 |            12796.05 |              2.69 | 1688.33 ms (13.2%) |    12401.32 |
|                |      4 GB |   196.22 |      21.89 |            26887.84 |              2.56 |  2381.63 ms (8.9%) |    27819.12 |
|                |      8 GB |   389.41 |      22.06 |            56318.16 |              2.44 |  4491.14 ms (8.0%) |    57769.37 |
| **24 × 1 MiB** |      1 MB |     0.06 |      17.76 |               13.31 |              1.26 |    1.91 ms (14.4%) |       12.47 |
|                |     64 MB |     3.00 |      22.35 |              273.73 |              3.92 |    14.44 ms (5.3%) |      276.08 |
|                |    256 MB |    11.94 |      22.49 |              975.95 |              4.40 |    37.18 ms (3.8%) |      977.41 |
|                |      1 GB |    47.67 |      22.52 |             3747.77 |              4.58 |   157.62 ms (4.2%) |     3762.01 |
|                |      2 GB |    95.27 |      22.54 |             7144.63 |              4.81 |   135.62 ms (1.9%) |     7124.75 |
|                |      4 GB |   194.45 |      22.09 |            14395.58 |              4.77 |   488.53 ms (3.4%) |    14344.29 |
|                |      8 GB |   389.96 |      22.03 |            30516.10 |              4.50 |  2484.97 ms (8.1%) |    31184.24 |
| **24 × 4 MiB** |      1 MB |     0.06 |      18.44 |               15.76 |              1.06 |    2.56 ms (16.3%) |       16.00 |
|                |     64 MB |     2.99 |      22.44 |              550.61 |              1.95 |    48.65 ms (8.8%) |      553.65 |
|                |    256 MB |    11.95 |      22.47 |             1675.88 |              2.56 |  192.55 ms (11.5%) |     1611.40 |
|                |      1 GB |    50.52 |      21.25 |             6030.52 |              2.85 |   321.43 ms (5.3%) |     5988.79 |
|                |      2 GB |    95.29 |      22.54 |            13271.68 |              2.59 | 2400.44 ms (18.1%) |    12564.77 |
|                |      4 GB |   195.49 |      21.97 |            29601.38 |              2.32 | 4279.68 ms (14.5%) |    31035.05 |
|                |      8 GB |   393.51 |      21.83 |            57824.54 |              2.38 |  4336.10 ms (7.5%) |    58849.56 |

# Testing stripe configurations on a Smilei simulation

## Smilei 2D stripe-layout results

| Stripe configuration | n | Time loop (s) | Diagnostics (s) | Diagnostics (%) | Fields0.h5 (s) | Wall time (s) | Network timeouts |
| -------------------- | -: | ------------: | --------------: | --------------: | -------------: | ------------: | ---------------: |
| 1 × 1 MiB            | 3 | 2080.0 ± 22.2 |   863.1 ± 18.0 |            41.3 |     786.7 ± 20.8 | 2088.3 ± 24.0 |       42.7 ± 2.9 |
| **2 × 1 MiB (default)** | 3 | 1676.5 ± 9.6 | 470.2 ± 9.3 |            28.0 |      430.0 ± 10.0 | 1684.0 ± 11.8 |       32.0 ± 3.5 |
| 4 × 1 MiB            | 3 | 1554.0 ± 2.9  |    313.9 ± 0.9 |            20.0 |       270.0 ± 0.0 | 1561.0 ± 4.4  |       10.0 ± 3.5 |
| 8 × 1 MiB            | 3 | 1490.9 ± 4.9  |    268.8 ± 1.4 |            18.0 |       230.0 ± 0.0 | 1498.0 ± 6.6  |       31.7 ± 9.3 |
| 16 × 1 MiB           | 3 | 1496.7 ± 9.0  |    266.9 ± 0.9 |            18.0 |       220.0 ± 0.0 | 1504.3 ± 9.5  |       21.7 ± 3.1 |
| 4 × 4 MiB            | 3 | 1521.4 ± 4.4  |    309.0 ± 2.0 |            20.0 |       260.0 ± 0.0 | 1529.0 ± 5.6  |       31.0 ± 8.7 |
| 8 × 4 MiB *          | 3 | 1502.3 ± 3.8  |    271.0 ± 0.2 |            18.0 |       220.0 ± 0.0 | 1509.0 ± 2.8  |       24.0 ± 0.0 |
| **16 × 4 MiB**       | 3 | **1491.6 ± 1.0** | **255.7 ± 1.0** | **17.0** | **200.0 ± 0.0** | **1500.0 ± 2.6** | 32.3 ± 6.1 |
| **24 × 4 MiB**       | 3 | **1502.1 ± 13.9** | **254.9 ± 10.1** | **17.3** | **200.0 ± 0.0** | **1508.3 ± 13.7** | 10.3 ± 3.8 |

- Fields0.h5 (s) is the time Smilei reports for the Fields0.h5 diagnostic in its Diagnostics profile. It is not a pure Lustre write-time measurement. It includes the work associated with that diagnostic, such as preparing/gathering the field data and writing/synchronizing the HDF5 output.
- Diagnostics (s) is the total time Smilei reports for all diagnostics combined during the simulation (in this case : Diagnostics time ≈ Fields0 diagnostic time + Performances diagnostic time + small miscellaneous diagnostic overhead).
- Every run writes the same amount of diagnostic data, the file size is constant: 541,304,202,776 bytes ≈ 541.3 GB ≈ 504.1 GiB.

## Re-run on best layout to confirm reproducibility / check Lustre contention 10 days later:

| Run | Repetitions | Stripe layout | Time loop (s) | Diagnostics (s) | Diagnostics (%) | Fields0.h5 (s) |
| --- | ---: | --- | ---: | ---: | ---: | ---: |
| Previous measurements | 3 | 16 × 4 MiB | 1491.6 ± 1.0 | 255.7 ± 1.0 | 17.0 | 200.0 ± 0.0 |
| Current rerun | 1 | 16 × 4 MiB | 1644.9 | 266.3 | 16.0 | 210.0 |

| Metric | Previous mean | Current rerun | Change |
| --- | ---: | ---: | ---: |
| Time loop | 1491.6 s | 1644.9 s | +10.3% |
| Diagnostics | 255.7 s | 266.3 s | +4.2% |
| Fields0.h5 | 200.0 s | 210.0 s | +5.0% |

## Smilei 3D stripe-layout results

Diagnostics frequency: `every = 5`

### 1 GPU: diagnostics = 15.6% of runtime (file size: 38.63GiB)

| Stripe config | Time loop (s) | Diagnostics (s) | Fields0.h5 (s) | Compute (s) | Diag/compute (%) | Fields file size (GiB) | Wall (s) |
|---|---:|---:|---:|---:|---:|---:|---:|
| 1 × 1 MiB | 318.25 | 57.39 | 53.00 | 260.86 | 22.00 | 38.63 | 403 |
| 2 × 1 MiB (default) | 314.44 | 52.66 | 49.00 | 261.78 | 20.11 | 38.63 | 400 |
| 4 × 1 MiB | 318.93 | 54.32 | 50.00 | 264.61 | 20.53 | 38.63 | 404 |
| 4 × 4 MiB | 313.89 | 54.78 | 51.00 | 259.11 | 21.14 | 38.63 | 399 |
| 8 × 1 MiB | 321.07 | 58.35 | 55.00 | 262.72 | 22.21 | 38.63 | 406 |
| 8 × 4 MiB | 316.00 | 58.13 | 54.00 | 257.87 | 22.54 | 38.63 | 401 |
| 16 × 1 MiB | 327.10 | 59.78 | 56.00 | 267.32 | 22.36 | 38.63 | 413 |
| 16 × 4 MiB | 323.58 | 63.67 | 60.00 | 259.91 | 24.50 | 38.63 | 408 |
| 24 × 4 MiB | 322.73 | 67.83 | 64.00 | 254.90 | 26.61 | 38.63 | 408 |

### 8 GPUs: diagnostics = 52.4% of runtime (file size: 305.43GiB)

| Stripe config | Time loop (s) | Diagnostics (s) | Fields0.h5 (s) | Compute (s) | Diag/compute (%) | Fields file size (GiB) | Wall (s) |
|---|---:|---:|---:|---:|---:|---:|---:|
| 1 × 1 MiB | 770.31 | 491.82 | 490.00 | 278.49 | 176.61 | 305.43 | 861 |
| 2 × 1 MiB (default) | 591.28 | 313.11 | 310.00 | 278.17 | 112.56 | 305.43 | 682 |
| 4 × 1 MiB | 578.78 | 300.29 | 300.00 | 278.49 | 107.83 | 305.43 | 668 |
| 4 × 4 MiB | 594.49 | 316.61 | 310.00 | 277.88 | 113.94 | 305.43 | 684 |
| 8 × 1 MiB | 612.99 | 327.11 | 320.00 | 285.88 | 114.43 | 305.43 | 702 |
| 8 × 4 MiB | 587.75 | 304.19 | 300.00 | 283.56 | 107.27 | 305.43 | 677 |
| 16 × 1 MiB | 623.52 | 330.06 | 330.00 | 293.46 | 112.47 | 305.43 | 718 |
| 16 × 4 MiB | 616.49 | 330.00 | 330.00 | 286.49 | 115.19 | 305.43 | 706 |
| 24 × 4 MiB | 629.41 | 333.89 | 330.00 | 295.52 | 112.98 | 305.43 | 721 |

## Smilei 3D re-run with larger domain (288³) on 8GPUs

| Stripe layout | Time loop (s) | Diagnostics (s) | Fields0 (s) | Compute (s) | Diagnostics / loop | Fields0 / loop | Wall time (s) | Status |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | --- |
| Default — 2 × 1 MiB | 848.37 | 462.71 | 460 | 385.66 | 54.54% | 54.22% | 963 | OK |
| **4 × 1 MiB** | **806.35** | **425.30** | **420** | 381.05 | **52.74%** | **52.09%** | **921** | OK |
| 8 × 4 MiB | 819.93 | 437.06 | 430 | 382.87 | 53.30% | 52.44% | 934 | OK |

| Stripe layout | Fields0 time | Effective write rate |
| --- | ---: | ---: |
| Default — 2 × 1 MiB | 460 s | ~1.01 GB/s |
| **4 × 1 MiB** | **420 s** | **~1.11 GB/s** |
| 8 × 4 MiB | 430 s | ~1.08 GB/s |

4 × 1 MiB remains the best tested Lustre layout. Compared with the default 2 × 1 MiB layout, it reduces field diagnostic time by ~8.7% and total wall time by ~4.4%.
Increasing further to 8 × 4 MiB remains better than default, but slightly worse than 4 × 1 MiB.

### Re-run with probe instead of fields diagnostics

| Parameter | Original value | I/O-heavy value |
|---|---:|---:|
| Grid size | `288 × 288 × 288` | `64 × 64 × 64` |
| `simulation_time` | `800 * dt` | `40 * dt` |
| `particles_per_cell` | `32` | `2` |
| `DiagScalar(every)` | `5` | `1` |
| Probe dimensionality | 2D | 3D |
| Probe resolution | `[32, 32]` | `[96, 96, 96]` |
| `DiagProbe(every)` | `5` | `1` |
| `flush_every` | not set | `1` |
| Probe datatype | default | `"double"` |

| Stripe config | Successful reps | Avg loop time (s) | Avg probe time (s) | Avg compute time (s) | Avg probe % loop | Probe size (GB) | Approx. throughput (GB/s) |
|---|---:|---:|---:|---:|---:|---:|---:|
| **2 × 1 MiB**  | 3 | 6.915 | 5.733 | 1.056 | 82.90% | 4.239 | 0.739 |
| **4 × 1 MiB**  | 3 | 7.127 | 5.933 | 1.049 | 83.25% | 4.239 | 0.714 |
| **8 × 1 MiB**  | 3 | 6.922 | 5.700 | 1.069 | 82.34% | 4.239 | **0.744** |
| **16 × 1 MiB** | 2 | 7.191 | 5.950 | 1.073 | 82.75% | 4.239 | 0.712 |
| **4 × 4 MiB**  | 3 | 7.148 | 5.967 | 1.059 | 83.47% | 4.239 | 0.710 |
| **8 × 4 MiB**  | 3 | 7.040 | 5.867 | 1.073 | 83.31% | 4.239 | 0.723 |
| **16 × 4 MiB** | 3 | 7.097 | 5.867 | 1.072 | 82.65% | 4.239 | 0.723 |

# PDI shared HDF5 results with different stripe layouts

| Stripe configuration | n | I/O time (s) | Total time (s) | I/O share (%) | Throughput (GiB/s) | Throughput (GB/s) | Wall time (s) | File size (GiB) |
| -------------------- | -: | -----------: | -------------: | ------------: | -----------------: | ----------------: | ------------: | --------------: |
| 1 × 1 MiB            | 3 | 324.21 ± 2.01 | 329.45 ± 2.00 | 98.41 ± 0.02 | 0.611 ± 0.004 | 0.656 ± 0.004 | 332.66 ± 1.81 | 198.000002 |
| **2 × 1 MiB (default)** | 6 | 183.51 ± 2.19 | 188.81 ± 2.16 | 97.20 ± 0.06 | 1.079 ± 0.013 | 1.159 ± 0.014 | 191.94 ± 2.15 | 198.000002 |
| 4 × 1 MiB            | 3 | 157.81 ± 1.60 | 163.08 ± 1.60 | 96.77 ± 0.03 | 1.255 ± 0.013 | 1.347 ± 0.014 | 166.37 ± 1.57 | 198.000002 |
| **8 × 1 MiB**        | 3 | **126.52 ± 0.99** | **131.83 ± 0.98** | **95.97 ± 0.04** | **1.565 ± 0.012** | **1.680 ± 0.013** | **135.13 ± 1.45** | 198.000002 |
| 16 × 1 MiB           | 3 | 155.66 ± 0.38 | 160.98 ± 0.38 | 96.69 ± 0.01 | 1.272 ± 0.003 | 1.366 ± 0.003 | 164.26 ± 0.66 | 198.000002 |
| 4 × 4 MiB            | 3 | 153.78 ± 1.72 | 159.06 ± 1.72 | 96.67 ± 0.04 | 1.288 ± 0.014 | 1.383 ± 0.016 | 162.22 ± 1.54 | 198.000002 |
| 8 × 4 MiB            | 3 | 134.07 ± 0.32 | 139.39 ± 0.34 | 96.18 ± 0.01 | 1.477 ± 0.003 | 1.586 ± 0.004 | 142.60 ± 0.48 | 198.000002 |
| 16 × 4 MiB           | 3 | 156.50 ± 0.49 | 162.29 ± 1.17 | 96.44 ± 0.46 | 1.265 ± 0.004 | 1.358 ± 0.004 | 165.40 ± 0.98 | 198.000002 |
| 24 × 4 MiB           | 3 | 227.96 ± 2.05 | 233.33 ± 2.06 | 97.70 ± 0.02 | 0.869 ± 0.008 | 0.933 ± 0.008 | 236.67 ± 2.49 | 198.000002 |
