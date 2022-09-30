#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

int main(int argc, char** argv)
{

#ifndef NDEBUG
	printf("Program starts  \n"); 
#endif
	int ierr;
	ierr = MPI_Init(&argc, &argv);  
	mpi_error_check(ierr); 

	MPI_Comm comm; 
	MPI_Comm_dup(MPI_COMM_WORLD,&comm); 

	int globalRank, globalSize; 
	MPI_Comm_rank(comm, &globalRank);
	MPI_Comm_size(comm, &globalSize); 

	/*
	 * Check if there are evenly matched IO Servers and Comp Servers 
	 */ 
	if (globalSize %2 != 0 || globalSize < 2)  
	{
		printf("Invalid globalSize. It needs to be an even number and greater than 1 \n"); 
		exit(1); 
	} 

	struct iocomp_params iocompParams; 
	intercommInit(&iocompParams, comm); 
#ifndef NDEBUG
	printf("After intercommInit\n"); 
#endif

	/*
	 * Initialise data for intercomm function 
	 */ 
	double* data = NULL; 
	data = initData(&iocompParams); 
#ifndef NDEBUG
	printf("After init_data \n"); 
#endif

	intercomm(comm,data,&iocompParams); 
	printf("after intercomm function\n"); 

	MPI_Finalize(); 
	printf("MPI finalize\n"); 
#ifndef NDEBUG
	printf("MPI finalize\n"); 
#endif

	free(data); 
	data = NULL; 

	return 0; 
} 

