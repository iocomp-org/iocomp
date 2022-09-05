#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

void computeServer(int NDIM, int* local_size, MPI_Comm computeComm, MPI_Comm interComm)
{

	int localDataSize = 1; 
	int i; 
	int computeRank, computeSize;
	for(i = 0; i < NDIM; i++)
	{
		localDataSize *= local_size[i]; 
	}

	int dest, tag; 	
	double* send_ar = NULL; 
	send_ar = (double*)malloc(localDataSize*sizeof(double)); //send array is divided by numnber of compute ranks 
	
	int ierr;
	ierr = MPI_Comm_rank(computeComm, &computeRank); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_size(computeComm, &computeSize); 
	mpi_error_check(ierr); 
	MPI_Request request; 
	
	for(i = 0; i < localDataSize; i++)
	{
		send_ar[i] = localDataSize * computeRank + i; 
	}

#ifndef NDEBUG
	printf("Sending data starts \n"); 
#endif

	dest = 0; // Assuming IO server is rank 0 
	tag = computeRank; // sending process's tag is its own rank
	
	ierr = MPI_Isend(send_ar, localDataSize , MPI_DOUBLE, dest, tag,
			interComm, &request); // every rank sends its portion of data 
	mpi_error_check(ierr); 

#ifndef NDEBUG
	printf("Sending data stop \n"); 
#endif

	free(send_ar);
	send_ar = NULL; 
} 

