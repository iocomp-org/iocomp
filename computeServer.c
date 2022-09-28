#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

void computeSendData(double* send_ar, int localDataSize, struct iocomp_params *iocompParams)
{
	int ierr, computeRank, computeSize;
	ierr = MPI_Comm_rank(iocompParams->compServerComm, &computeRank); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_size(iocompParams->compServerComm, &computeSize); 
	mpi_error_check(ierr); 
	MPI_Request request;

	int tag = computeRank;

#ifndef NDEBUG
	printf("Sending data starts from computeRank %i  \n", computeRank); 
#endif
	ierr = MPI_Isend(send_ar, localDataSize , MPI_DOUBLE, iocompParams->ioServerRank, tag,
			iocompParams->interComm, &request); // every rank sends its portion of data 
	mpi_error_check(ierr); 


#ifndef NDEBUG
	printf("Sending data stop \n"); 
#endif
}

void computeServer(int NDIM, double* data, int* local_size,  struct iocomp_params *iocompParams)
{

	int localDataSize = 1; 
	int i; 
	int computeRank, computeSize;
	for(i = 0; i < NDIM; i++)
	{
		localDataSize *= local_size[i]; 
	}

	int ierr;
	ierr = MPI_Comm_rank(iocompParams->compServerComm, &computeRank); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_size(iocompParams->compServerComm, &computeSize); 
	mpi_error_check(ierr); 
	MPI_Request request; 

#ifndef NDEBUG
	printf("Sending data starts \n"); 
#endif
	computeSendData(data, localDataSize, iocompParams); 
} 

