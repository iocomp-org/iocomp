#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

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

	int ioSize,ioRank, i; 
	MPI_Comm_size(comm, &ioSize);
	MPI_Comm_rank(comm, &ioRank);
	
	/*
	 * Array size specs initialise
	 */ 
	iocompParams->localArray = malloc(sizeof(size_t)*iocompParams->NDIM);
	iocompParams->globalArray = malloc(sizeof(size_t)*iocompParams->NDIM);
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
		iocompParams->localArray[i]	= (int)pow(iocompParams->localDataSize,power); 
		iocompParams->globalArray[i]	= iocompParams->localArray[i]; 
	}

	/*
	 * globalArray is multiplied by total size of available processors 
	 * since comm is dependant on hyperthread flag, if HT flag is ON, global size is half 
	 * no need for if statements 
	 */ 
	iocompParams->globalArray[0]*= ioSize; // assumes outermost dimension gets expanded by each rank 

#ifndef NDEBUG
	printf("arrayParamsInit -> localArray initialised \n"); 
	for (int i = 0; i < iocompParams->NDIM; i++){printf("%li ", iocompParams->localArray[i]); }
	printf("\narrayParamsInit -> globalArray initialised with size \n"); 
	for (int i = 0; i < iocompParams->NDIM; i++){printf("%li ", iocompParams->globalArray[i]); }
	printf("\narrayParamsInit -> NDIM initialised with size %i\n", iocompParams->NDIM); 
#endif

	/*
	 * Array local, global data sizes initialise 
	 */ 
	iocompParams->globalDataSize = iocompParams->localDataSize * ioSize; 
	
#ifndef NDEBUG
	printf("arrayParamsInit -> size definitions, localDataSize %li, globalDataSize %li\n", iocompParams->localDataSize, iocompParams->globalDataSize); 
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
	iocompParams->arrayStart[0] = ioRank * iocompParams->localArray[0]; // assuming ar_size has uniform dimensions. 
#ifndef NDEBUG
	printf("arrayParamsInit -> arrayStart initialised ioRank = %i\n",ioRank);
#endif

} 
