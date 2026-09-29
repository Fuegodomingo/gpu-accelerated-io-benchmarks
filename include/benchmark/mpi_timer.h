#pragma once

/** @file mpi_timer.h @brief Cross-rank wall-clock timing helpers. */

#include <mpi.h>

/** @brief Barrier and return the local MPI timestamp starting a timed region. */
double mpi_region_begin(MPI_Comm comm);

/** @brief End a timed region and reduce slowest-rank wall time to rank 0. */
double mpi_region_end_ms(double t0, MPI_Comm comm);
