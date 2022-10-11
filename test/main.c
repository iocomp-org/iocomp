#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"
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
	int localArraySize[2] = {10,10}; 

	struct iocomp_params iocompParams; 

	comm_split(&iocompParams, comm); 

	iocompInit(&iocompParams, comm,  NDIM, localArraySize); 
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

	intercomm(&iocompParams); // do io otherwise do nothing  
	computeStep(data,&iocompParams); // do compute 
	computeServer(data,&iocompParams); // send data off using computeServer

	MPI_Finalize(); 
#ifndef NDEBUG
	printf("MPI finalize\n"); 
#endif

	free(data); 
	data = NULL; 

	return 0; 
} 

double* initData(struct iocomp_params *iocompParams)
{
	double* data = NULL; 
	data = (double*)malloc(iocompParams->localDataSize*sizeof(double)); // one rank only sends to one rank
	int i, globalMPIRank, ierr;   

	ierr = MPI_Comm_rank(iocompParams->globalComm, &globalMPIRank); 
	mpi_error_check(ierr); 
	for(i = 0; i < iocompParams->localDataSize; i++)
	{
		data[i] =iocompParams->localDataSize * globalMPIRank + i; 
	}
	return data; 
} 

