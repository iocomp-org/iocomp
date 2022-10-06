#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

int main(int argc, char** argv)
{

	int ierr;
	ierr = MPI_Init(&argc, &argv);  
	mpi_error_check(ierr); 

	MPI_Comm comm; 
	MPI_Comm_dup(MPI_COMM_WORLD,&comm); 
	// data parameters definitions 

	int NDIM = 2; 
	int localArraySize[2] = {10,10}; 
 
	struct iocomp_params iocompParams; 
	 
	intercommInit(&iocompParams, comm,  NDIM, localArraySize); 
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

	if(iocompParams.colour == ioColour)
	{
		intercomm(comm,data,&iocompParams); 
		MPI_Finalize(); 
		return(0); 
	} 
 
	computeStep(data,&iocompParams); 

	MPI_Finalize(); 
#ifndef NDEBUG
	printf("MPI finalize\n"); 
#endif

	free(data); 
	data = NULL; 

	return 0; 
} 

