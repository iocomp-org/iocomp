#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

#define N 10 
#define NDIM 3 
#define IO_SERVER_SIZE 1 
int main(int argc, char** argv)
{

#ifndef NDEBUG
	printf("before mpi init main program \n"); 
#endif
	int ierr;
	ierr = MPI_Init(&argc, &argv);  
	mpi_error_check(ierr); 
	
	/*
	* Initialise data for intercomm function 
	*/ 
	double* data = NULL; 
	data = init_data(N, NDIM, MPI_COMM_WORLD); 

#ifndef NDEBUG
	printf("After init data \n"); 
#endif

	intercomm(MPI_COMM_WORLD,data,IO_SERVER_SIZE); 
	
#ifndef NDEBUG
	printf("After intercomm program \n"); 
#endif

	MPI_Finalize(); 

#ifndef NDEBUG
	printf("MPI finalize\n"); 
#endif

	return 0; 
} 

