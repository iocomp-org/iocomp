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
void arrayParamsInit(struct iocomp_params *iocompParams, MPI_Comm comm )
{

#ifndef NDEBUG
	printf("arrayParamsInit -> start \n"); 
#endif

	int ioSize,ioRank, ierr, i;
	MPI_Comm_size(comm, &ioSize);
	MPI_Comm_rank(comm, &ioRank);
	
	/*
	 * Array size specs initialise
	 */ 
	iocompParams->localSize = malloc(sizeof(size_t)*iocompParams->NDIM);
	iocompParams->globalSize = malloc(sizeof(size_t)*iocompParams->NDIM);
	iocompParams->dataType = MPI_DOUBLE; // data type of sent and recvd data 
#ifndef NDEBUG
	printf("arrayParamsInit -> local, global size initialised \n"); 
#endif

	/*
	 * Local and global sizes are initialised 
	 * if NDIM = 2, then localDataSize 64 gives array of 8,8
	 */ 
	float power = (float)1/iocompParams->NDIM; 
	for (i = 0; i < iocompParams->NDIM; i++)
	{
		//iocompParams->localSize[i]	= (int)sqrt(iocompParams->localDataSize); 
		//iocompParams->globalSize[i]	= (int)sqrt(iocompParams->localDataSize); 
		iocompParams->localSize[i]	= (int)pow(iocompParams->localDataSize,power); 
		iocompParams->globalSize[i]	= iocompParams->localSize[i]; 
	}

	/*
	 * globalSize is multiplied by total size of available processors 
	 * since comm is dependant on hyperthread flag, if HT flag is ON, global size is half 
	 * no need for if statements 
	 */ 
	iocompParams->globalSize[0]*= ioSize; // assumes outermost dimension gets expanded by each rank 

#ifndef NDEBUG
	printf("arrayParamsInit -> localsize initialised \n"); 
	for (int i = 0; i < iocompParams->NDIM; i++){printf("%i ", iocompParams->localSize[i]); }
	printf("\narrayParamsInit -> globalsize initialised with size \n"); 
	for (int i = 0; i < iocompParams->NDIM; i++){printf("%i ", iocompParams->globalSize[i]); }
	printf("\narrayParamsInit -> NDIM initialised with size %i\n", iocompParams->NDIM); 
#endif

	/*
	 * Array local, global data sizes initialise 
	 */ 
	iocompParams->globalDataSize = iocompParams->localDataSize * ioSize; 
	
#ifndef NDEBUG
	printf("arrayParamsInit -> size definitions, localDataSize %i, globalDataSize %i\n", iocompParams->localDataSize, iocompParams->globalDataSize); 
#endif

	/*
	 * Define and initialise arrayStart
	 */ 
	iocompParams->arrayStart = malloc(sizeof(int)*iocompParams->NDIM);
#ifndef NDEBUG
	printf("arrayParamsInit -> initialise arrayStart \n");
#endif

	for (int i = 0; i < iocompParams->NDIM; i++)
	{
		iocompParams->arrayStart[i] = 0; 
	}
	iocompParams->arrayStart[0] = ioRank * iocompParams->localSize[0]; // assuming ar_size has uniform dimensions. 
#ifndef NDEBUG
	printf("arrayParamsInit -> arrayStart initialised ioRank = %i\n",ioRank);
#endif

} 
