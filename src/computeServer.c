#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

void computeServer(double* data, struct iocomp_params *iocompParams)
{

	int i, ierr, computeRank, computeSize;

	ierr = MPI_Comm_rank(iocompParams->compServerComm, &computeRank); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_size(iocompParams->compServerComm, &computeSize); 
	mpi_error_check(ierr); 
	MPI_Request request;

#ifndef NDEBUG
	printf("Sending data starts from computeRank %i to ioRank %i  \n", computeRank, computeRank); 
#endif
	/*
	 *	Send data using MPI_Isend to computeRank in interComm  
	 *	which is paired with the same rank of IO server 
	 */
	int dest, tag; 
	tag = dest = computeRank;

	ierr = MPI_Isend(data, iocompParams->localDataSize , iocompParams->dataType, dest, tag,
			iocompParams->interComm, &request); // every rank sends its portion of data 
	mpi_error_check(ierr); 

#ifndef NDEBUG
	printf("Sending data stop \n"); 
#endif
} 

