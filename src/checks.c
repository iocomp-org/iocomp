#include "../include/iocomp.h"
#include <stdlib.h>
#include "stdio.h"

void mpi_error_check(int ierr)
{
	if (ierr != MPI_SUCCESS)
	{
		printf("MPI call fails \n"); 
		exit(0); 
	}
}

void malloc_check(double* test)
{
	if(test == NULL)
	{	
		printf("Malloc fails, exiting. \n"); 
		exit(1); 
	}

}

void free_check(double* test)
{
	if(test != NULL)
	{
		printf("free fails, exiting \n"); 
		exit(1); 
	}

}

