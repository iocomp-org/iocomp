#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include <math.h>
#include <sys/time.h>
#include "iocomp.h"
#include "bench_headerfiles.h"
//#include <hdf5.h>
//#include <H5FDmpio.h>
//#include <H5FDmpi.h>
//#include <hdf5_hl.h>
//#include <adios2_c.h>
//#include <adios2/c/adios2_c_types.h>

//#include "bench_headerfiles.h"

#define NDIM2 2
#define n 10

void bench_init(double *iodata, int NDIM, int *local_size, int *global_size, int *array_start, MPI_Comm comm)
{
	int reorder = 0,
		size, rank,
		total_size,
		coords[NDIM],
		periods[NDIM],
		dims_mpi[NDIM];

	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

	double start, end; // timer variables

	double global_data_size;

	global_data_size = 1;

	for (int i = 0; i < NDIM; i++)
	{
		global_data_size *= global_size[i];
	}

	for (int j = 0; j < NDIM; j++)
	{
		dims_mpi[j] = 0;
		periods[j] = 0;
		coords[j] = 0;
	}
	char *filename = "mpiio.dat";

#ifndef NDEBUG
	printf("size dimensions size of dataset %i %i \n", local_size[0], local_size[1]);
#endif
#ifndef NDEBUG
	printf("global dimensions size of dataset %i %i \n", global_size[0], global_size[1]);
#endif
#ifndef NDEBUG
	printf("array start dimensions size of dataset %i %i\n", array_start[0], array_start[1]);
#endif

	/* new communicator to which topology information is added */
	MPI_Comm cartcomm;
#ifndef NDEBUG
	printf("MPI new cartcomm \n");
#endif

	MPI_Dims_create(size, NDIM, dims_mpi);
#ifndef NDEBUG
	printf("MPI dims create \n");
#endif
	MPI_Cart_create(comm, NDIM, dims_mpi, periods, reorder, &cartcomm); // comm;
#ifndef NDEBUG
	printf("MPI cart create  \n");
#endif
	MPI_Cart_coords(cartcomm, rank, NDIM, coords);
#ifndef NDEBUG
	printf("MPI cart coords \n");
#endif

	// MPI_Barrier(ioServeComm);
	start = MPI_Wtime(); // start of timer
						 // phdf5write(iodata, local_size, global_size, arraystart, NDIM, cartcomm, "hdf5.h5");
						 // adioswrite(iodata, local_size, global_size, arraystart, NDIM, cartcomm, "BP4", "output.bp4");
	mpiiowrite(iodata, local_size, global_size, array_start, NDIM, cartcomm, filename);
	end = MPI_Wtime(); // end of timer

#ifndef NDEBUG
	printf("Writing timing ended \n");
#endif

	int irep = 1;
	int MAXLOOP_AVGIO = 1;
	timing_int(end - start, global_data_size, irep, MAXLOOP_AVGIO, local_size[0], filename, cartcomm);

#ifndef NDEBUG
	printf("Timing function ended  \n");
#endif
}
