/** @file mpi_timer.cpp @brief MPI wall-clock timing implementation. */

#include "benchmark/mpi_timer.h"
#include "validation/runtime_checks.h"

double mpi_region_begin(MPI_Comm comm)
{
    MPI_CHECK(MPI_Barrier(comm));
    return MPI_Wtime();
}

double mpi_region_end_ms(double t0, MPI_Comm comm)
{
    MPI_CHECK(MPI_Barrier(comm));
    const double local_ms = (MPI_Wtime() - t0) * 1e3;
    double wall_ms = 0.0;
    MPI_CHECK(MPI_Reduce(&local_ms, &wall_ms, 1, MPI_DOUBLE, MPI_MAX, 0, comm));
    return wall_ms;
}
