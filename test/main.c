#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "test.h"


int main(int argc, char** argv)
{

	int ierr;
	ierr = MPI_Init(&argc, &argv);  
	mpi_error_check(ierr); 

	MPI_Comm comm; 
	MPI_Comm_dup(MPI_COMM_WORLD,&comm); 
	// data parameters definitions 

	int NDIM = 2; 
	int localArraySize[2] = {4,4}; 

	struct iocomp_params iocompParams; 

	iocompInit(&iocompParams, comm,  NDIM, localArraySize); 
#ifndef NDEBUG
	printf("After intercommInit\n"); 
#endif
	
	double* data = NULL; // initialise data pointer  
	data = (double*)malloc(iocompParams.localDataSize*sizeof(double)); // one rank only sends to one rank
	computeStep(data,&iocompParams); // do compute 
#ifndef NDEBUG
	printf("after computeStep \n"); 
#endif

#ifndef NDEBUG
	printf("after computeServer \n"); 
#endif

	MPI_Finalize(); 
#ifndef NDEBUG
	printf("MPI finalize\n"); 
#endif

	free(data); 
	data = NULL; 

	return 0; 
} 

