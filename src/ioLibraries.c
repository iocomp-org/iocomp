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
	printf("ioLibraries -> start\n");
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
	 * Initiliase filename 
	 */ 
	iocompParams->FILENAMES[0] = "mpiio.dat"; 
	iocompParams->FILENAMES[1] = "hdf5.h5"; 
	iocompParams->FILENAMES[2] = "adios2.h5";
	iocompParams->FILENAMES[3] = "adios2.bp4";
	iocompParams->FILENAMES[4] = "adios2.bp5"; 


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
	double writeTime = 0.0; 

	/* new communicator to which topology information is added */
	MPI_Comm cartcomm;
#ifndef NDEBUG
	printf("ioLibraries -> MPI new cartcomm \n");
#endif

	MPI_Dims_create(ioSize, iocompParams->NDIM, dims_mpi);
#ifndef NDEBUG
	printf("ioLibraries -> MPI dims create \n");
#endif

	MPI_Cart_create(comm, iocompParams->NDIM, dims_mpi, periods, reorder, &cartcomm); // comm;
#ifndef NDEBUG
	printf("ioLibraries -> MPI cart create  \n");
#endif

	MPI_Cart_coords(cartcomm, ioRank, iocompParams->NDIM, coords);
#ifndef NDEBUG
	printf("ioLibraries -> MPI cart coords \n");
#endif
	MPI_Barrier(comm);
	double timerStart = 0.0; 
	
#ifndef NDEBUG
	printf("ioLibraries -> Pass off to I/O libraries with ioLibNum %i  \n", iocompParams->ioLibNum);
#endif

	switch(iocompParams->ioLibNum){

		case 0:
			if (!ioRank) {timerStart = MPI_Wtime();} 
			mpiiowrite(iodata, iocompParams->localSize, iocompParams->globalSize, iocompParams->arrayStart, iocompParams->NDIM, cartcomm, filename, iocompParams->dataType);
			MPI_Barrier(comm);
			if (!ioRank) {writeTime = MPI_Wtime() - timerStart;} 
		break; 

		case 1: 
			if (!ioRank) {timerStart = MPI_Wtime();} 
			phdf5write(iodata, iocompParams->localSize, iocompParams->globalSize, iocompParams->arrayStart, iocompParams->NDIM, cartcomm, "hdf5.h5");
			MPI_Barrier(comm);
			if (!ioRank) {writeTime = MPI_Wtime() - timerStart;} 
		break; 
	
		case 2: 
			if (!ioRank) {timerStart = MPI_Wtime();} 
			adioswrite(iodata, iocompParams->localSize, iocompParams->globalSize, iocompParams->arrayStart, iocompParams->NDIM, cartcomm, "HDF5", "adios2.h5");
			MPI_Barrier(comm);
			if (!ioRank) {writeTime = MPI_Wtime() - timerStart;} 
		break; 
	
		case 3: 
			if (!ioRank) {timerStart = MPI_Wtime();} 
			adioswrite(iodata, iocompParams->localSize, iocompParams->globalSize, iocompParams->arrayStart, iocompParams->NDIM, cartcomm, "BP4", "adios2.bp4");
			MPI_Barrier(comm);
			if (!ioRank) {writeTime = MPI_Wtime() - timerStart;} 
		break;

		case 4: 
			if (!ioRank) {timerStart = MPI_Wtime();} 
			adioswrite(iodata, iocompParams->localSize, iocompParams->globalSize, iocompParams->arrayStart, iocompParams->NDIM, cartcomm, "BP5", "adios2.bp5");
			MPI_Barrier(comm);
			if (!ioRank) {writeTime = MPI_Wtime() - timerStart;} 
		break; 

		default:
			printf("Invalid I/O library number \n"); 
			break; 
	} 
		
#ifndef NDEBUG
	printf("ioLibraries -> end of switch for IO libraries\n");
#endif
	if (!ioRank) 
	{
		double fileSize = iocompParams->globalDataSize*sizeof(double)/(pow(10,9)); 
		printf("** I/O write time=%lf filesize(GB)=%lf\n", writeTime,fileSize) ; 
	} 

#ifndef NDEBUG
	printf("ioLibraries -> Timing function ended  \n");
#endif
}

