#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

#define N 10 
#define NDIM 2
#define IO_SERVER_SIZE 1 
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
	
	/*
	* Initialise data for intercomm function 
	*/ 
	double* data = NULL; 
	data = init_data(N, NDIM, comm); 

#ifndef NDEBUG
	printf("After init_data \n"); 
#endif
	
	struct iocomp_params iocompParams; 
	intercomm_init(&iocompParams, comm); 
	intercomm(comm,data,&iocompParams); 
	
	MPI_Finalize(); 

#ifndef NDEBUG
	printf("MPI finalize\n"); 
#endif

	return 0; 
} 

