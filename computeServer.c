#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

void computeServer(int NDIM, double* data, int* local_size,  struct iocomp_params *iocompParams)
{

	int localDataSize = 1; 
	int i, ierr, computeRank, computeSize;
	for(i = 0; i < NDIM; i++)
	{
		localDataSize *= local_size[i]; 
	}

#ifndef NDEBUG
	printf("Sending data starts \n"); 
#endif

	ierr = MPI_Comm_rank(iocompParams->compServerComm, &computeRank); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_size(iocompParams->compServerComm, &computeSize); 
	mpi_error_check(ierr); 
	MPI_Request request;

	int tag = iocompParams->compServerRank;

#ifndef NDEBUG
	printf("Sending data starts from computeRank %i to ioRank %i  \n", computeRank, 
			iocompParams->ioServerRank); 
#endif
	/*
	 *	Send data using MPI_Isend to computeRank in interComm  
	 *	which is paired with the same rank of IO server 
	 */
	int dest = computeRank; 
	ierr = MPI_Isend(data, localDataSize , MPI_DOUBLE, dest, tag,
			iocompParams->interComm, &request); // every rank sends its portion of data 
	mpi_error_check(ierr); 

#ifndef NDEBUG
	printf("Sending data stop \n"); 
#endif
} 

