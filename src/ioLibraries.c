#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include <math.h>
#include <sys/time.h>
#include "../include/iocomp.h"

void ioLibraries(double* iodata, struct iocomp_params *iocompParams)
{
#ifndef NDEBUG
	printf("Started ioLibraries\n");
#endif
	int reorder = 0; 
	int ioSize, ioRank; 
	int coords[iocompParams->NDIM], periods[iocompParams->NDIM],dims_mpi[iocompParams->NDIM];
	
	MPI_Comm comm; 
	if(iocompParams->hyperthreadFlag)
	{
		comm = iocompParams->ioServerComm; 
	}
	else
	{
		comm = iocompParams->globalComm; 
	}

	MPI_Comm_size(comm, &ioSize);
	MPI_Comm_rank(comm, &ioRank);

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

	/* new communicator to which topology information is added */
	MPI_Comm cartcomm;
#ifndef NDEBUG
	printf("MPI new cartcomm \n");
#endif

	MPI_Dims_create(ioSize, iocompParams->NDIM, dims_mpi);
#ifndef NDEBUG
	printf("MPI dims create \n");
#endif

	MPI_Cart_create(comm, iocompParams->NDIM, dims_mpi, periods, reorder, &cartcomm); // comm;
#ifndef NDEBUG
	printf("MPI cart create  \n");
#endif

	MPI_Cart_coords(cartcomm, ioRank, iocompParams->NDIM, coords);
#ifndef NDEBUG
	printf("MPI cart coords \n");
#endif
	MPI_Barrier(comm);
	double timerStart; 
	if (!ioRank) {timerStart = MPI_Wtime();} 
	mpiiowrite(iodata, iocompParams->localSize, iocompParams->globalSize, iocompParams->arrayStart, iocompParams->NDIM, cartcomm, filename, iocompParams->dataType);
	MPI_Barrier(comm);
	if (!ioRank) {iocompParams->writeTime[0][0] = MPI_Wtime() - timerStart;} 

		//if (!rank) {timerStart = MPI_Wtime();} 
		//phdf5write(iodata, iocompParams->localSize, iocompParams->globalSize, iocompParams->arrayStart, iocompParams->NDIM, cartcomm, "hdf5.h5");
		//MPI_Barrier(comm);
		//if (!rank) {iocompParams->timer[1][k] = MPI_Wtime() - timerStart;} 
		//
		//if (!rank) {timerStart = MPI_Wtime();} 
		////adioswrite(iodata, local_size, iocompParams->globalSize, arraystart, iocompParams->NDIM, cartcomm, "BP4", "output.bp4");
		//MPI_Barrier(comm);
		//if (!rank) {iocompParams->timer[2][k] = MPI_Wtime() - timerStart;} 
#ifndef NDEBUG
	printf("Writing timing ended \n");
#endif
		
		printf("** write time for library is %lf \n", iocompParams->writeTime[0][0]); 
		if (!ioRank) 
		{
			data_output(iocompParams); 
		} 
	//	int irep = 1;
	//	int MAXLOOP_AVGIO = 1;
	//	timer(iocompParams); 
	//	timing_int(end - start, global_data_size, irep, MAXLOOP_AVGIO, iocompParams->localSize[0], filename, cartcomm);

#ifndef NDEBUG
	printf("Timing function ended  \n");
#endif
}
