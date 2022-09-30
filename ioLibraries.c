#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include <math.h>
#include <sys/time.h>
#include "iocomp.h"
#define n 10

//void io_libraries(double *iodata, int NDIM, int *local_size, int *global_size, int *array_start, MPI_Comm comm)
void ioLibraries(double* iodata, struct iocomp_params *iocompParams)
{
#ifndef NDEBUG
	printf("Started ioLibraries\n");
#endif
	int reorder = 0,
		size, rank,
		total_size,
		coords[iocompParams->NDIM],
		periods[iocompParams->NDIM],
		dims_mpi[iocompParams->NDIM];

	MPI_Comm_size(iocompParams->ioServerComm, &size);
	MPI_Comm_rank(iocompParams->ioServerComm, &rank);

	double start, end; // timer variables

	double global_data_size;

	global_data_size = 1;

	for (int i = 0; i < iocompParams->NDIM; i++)
	{
		global_data_size *= iocompParams->globalSize[i];
	}

	for (int j = 0; j < iocompParams->NDIM; j++)
	{
		dims_mpi[j] = 0;
		periods[j] = 0;
		coords[j] = 0;
	}
	char *filename = "mpiio.dat";

#ifndef NDEBUG
	printf("size dimensions size of dataset %i %i \n", iocompParams->localSize[0], iocompParams->localSize[1]);
	printf("global dimensions size of dataset %i %i \n", iocompParams->globalSize[0], iocompParams->globalSize[1]);
	printf("array start dimensions size of dataset %i %i\n", iocompParams->arrayStart[0],iocompParams->arrayStart[1]);
#endif

	/* new communicator to which topology information is added */
	MPI_Comm cartcomm;
#ifndef NDEBUG
	printf("MPI new cartcomm \n");
#endif

	MPI_Dims_create(size, iocompParams->NDIM, dims_mpi);
#ifndef NDEBUG
	printf("MPI dims create \n");
#endif

	MPI_Cart_create(iocompParams->ioServerComm, iocompParams->NDIM, dims_mpi, periods, reorder, &cartcomm); // comm;
#ifndef NDEBUG
	printf("MPI cart create  \n");
#endif

	MPI_Cart_coords(cartcomm, rank, iocompParams->NDIM, coords);
#ifndef NDEBUG
	printf("MPI cart coords \n");
#endif

	// MPI_Barrier(ioServeComm);
						 // phdf5write(iodata, local_size, iocompParams->globalSize, arraystart, iocompParams->NDIM, cartcomm, "hdf5.h5");
						 // adioswrite(iodata, local_size, iocompParams->globalSize, arraystart, iocompParams->NDIM, cartcomm, "BP4", "output.bp4");
	mpiiowrite(iodata, iocompParams->localSize, iocompParams->globalSize, iocompParams->arrayStart, iocompParams->NDIM, cartcomm, filename);

#ifndef NDEBUG
	printf("Writing timing ended \n");
#endif

//	int irep = 1;
//	int MAXLOOP_AVGIO = 1;
//	timer(iocompParams); 
//	timing_int(end - start, global_data_size, irep, MAXLOOP_AVGIO, iocompParams->localSize[0], filename, cartcomm);

#ifndef NDEBUG
	printf("Timing function ended  \n");
#endif
}
