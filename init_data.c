#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

double* init_data(int N, int NDIM, MPI_Comm globalComm)
{
	int i, globalRank;
	int ierr;
	ierr = MPI_Comm_rank(globalComm, &globalRank); 
	mpi_error_check(ierr);

	double* data = NULL; 
	int localDataSize =1 ; 
	for(i = 0; i< NDIM; i++)
	{
		localDataSize *= N; 
	}
	data = (double*)malloc(localDataSize*sizeof(double)); //send array is divided by numnber of compute ranks 

	for(i = 0; i < localDataSize; i++)
	{
		data[i] = localDataSize * globalRank + i; 
	}
	return data; 
} 
