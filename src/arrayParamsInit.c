#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

#define fullNode 256 
#define HIGH_LOW 1
#define FIXED_IO_RANK 0

/*
 * Initialises the iocomp_params struct 
 */
void arrayParamsInit(struct iocomp_params *iocompParams, MPI_Comm comm, int NDIM, int* localArraySize)
{

#ifndef NDEBUG
	printf("Start of intercomm_init\n"); 
#endif

	int globalRank, globalSize, ierr, i;
	MPI_Comm_rank(comm, &globalRank);
	MPI_Comm_size(comm, &globalSize); 
	
	/*
	 * Array size specs initialise
	 */ 
	
	iocompParams->localSize = malloc(sizeof(int)*NDIM);
	iocompParams->globalSize = malloc(sizeof(int)*NDIM);
	iocompParams->NDIM = NDIM;// number of dimensions  
	//iocompParams->dataType = MPI_INT; // data type of sent and recvd data 
	iocompParams->dataType = MPI_DOUBLE; // data type of sent and recvd data 

	for (i = 0; i < iocompParams->NDIM; i++)
	{
		iocompParams->localSize[i] = localArraySize[i]; // initialise based on passed argument  
		iocompParams->globalSize[i] = localArraySize[i]; 
	}
	if(iocompParams->hyperthreadFlag)
	{
		iocompParams->globalSize[0]*= globalSize/2; // assumes outermost dimension gets expanded by each rank  
	}
	else 
	{
		iocompParams->globalSize[0]*= globalSize; // assumes outermost dimension gets expanded by each rank  
	}
#ifndef NDEBUG
	printf("localSize, globalSize, arrayStart initialised\n"); 
#endif

	/*
	 * Array local, global data sizes initialise 
	 */ 

	iocompParams->localDataSize = 1; 
	iocompParams->globalDataSize = 1; 
	
	for (i = 0; i < iocompParams->NDIM; i++)
	{
		iocompParams->localDataSize *= iocompParams->localSize[i]; 
		iocompParams->globalDataSize *= iocompParams->globalSize[i]; 
	} 
#ifndef NDEBUG
	printf("size definitions, localDataSize %i, globalDataSize %i\n", iocompParams->localDataSize, iocompParams->globalDataSize); 
#endif


#ifndef NDEBUG
	printf("End intercomm_init\n"); 
#endif

} 
