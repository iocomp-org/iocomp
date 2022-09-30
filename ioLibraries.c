#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include <math.h>
#include <sys/time.h>
#include "iocomp.h"

void ioLibraries(double* iodata, struct iocomp_params *iocompParams)
{
#ifndef NDEBUG
	printf("Started ioLibraries\n");
#endif
	int reorder = 0,
		ioSize, ioRank,
		total_size,
		coords[iocompParams->NDIM],
		periods[iocompParams->NDIM],
		dims_mpi[iocompParams->NDIM];

	MPI_Comm_size(iocompParams->ioServerComm, &ioSize);
	MPI_Comm_rank(iocompParams->ioServerComm, &ioRank);
	
	/*
	* Define and initialise arrayStart
	*/ 
	iocompParams->arrayStart = malloc(sizeof(int)*iocompParams->NDIM);
	for (int i = 0; i < iocompParams->NDIM; i++)
	{
		iocompParams->arrayStart[i] = 0; 
	}
	iocompParams->arrayStart[0] = ioRank * iocompParams->localSize[0]; // assuming ar_size has uniform dimensions. 

	double start, end; // timer variables

	for (int j = 0; j < iocompParams->NDIM; j++)
	{
		dims_mpi[j] = 0;
		periods[j] = 0;
		coords[j] = 0;
	}
	char *filename = "mpiio.dat";

#ifndef NDEBUG
	printf("printing localsize \n"); 
	for (int i = 0; i < iocompParams->NDIM; i++)
	{
		printf("%i ", iocompParams->localSize[i]); 
	}
	printf("\n"); 
	printf("printing globalSize \n"); 
	for (int i = 0; i < iocompParams->NDIM; i++)
	{
		printf("%i ", iocompParams->globalSize[i]); 
	}
	printf("\n"); 
	printf("printing arrayStart \n"); 
	for (int i = 0; i < iocompParams->NDIM; i++)
	{
		printf("%i ", iocompParams->arrayStart[i]); 
	}
#endif

	/* new communicator to which topology information is added */
	MPI_Comm cartcomm;
#ifndef NDEBUG
	printf("MPI new cartcomm \n");
#endif

	MPI_Dims_create(ioSize, iocompParams->NDIM, dims_mpi);
#ifndef NDEBUG
	printf("MPI dims create \n");
#endif

	MPI_Cart_create(iocompParams->ioServerComm, iocompParams->NDIM, dims_mpi, periods, reorder, &cartcomm); // comm;
#ifndef NDEBUG
	printf("MPI cart create  \n");
#endif

	MPI_Cart_coords(cartcomm, ioRank, iocompParams->NDIM, coords);
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
