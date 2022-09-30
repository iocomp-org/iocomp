#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"
#define N 10 
#define NDIM_define 2 // change this parameter to change dimensions of data set  

double* initData(struct iocomp_params *iocompParams)
{
	/*
	* Initialise MPI stuff 
	*/ 
#ifndef NDEBUG
	printf("Start of initData\n"); 
#endif
	int i, ierr, globalMPIRank, globalMPISize;
	ierr = MPI_Comm_rank(iocompParams->globalComm, &globalMPIRank); 
	mpi_error_check(ierr);
	ierr = MPI_Comm_rank(iocompParams->globalComm, &globalMPISize); 
	mpi_error_check(ierr);

#ifndef NDEBUG
	printf("MPI ranks and sizes\n"); 
#endif
	/*
	 * Datasize specs initialise
	 */ 
	int localSize_local[NDIM_define];
	int globalSize_local[NDIM_define]; 
	iocompParams->localSize = localSize_local;  
	iocompParams->globalSize = globalSize_local;  
	iocompParams->NDIM = NDIM_define;// number of dimensions  

#ifndef NDEBUG
	printf("assign localSize, globalSize\n"); 
#endif
	for (int i = 0; i < iocompParams->NDIM; i++)
	{
		iocompParams->localSize[i] = N; 
		iocompParams->globalSize[i] = N; 
	}
	
	iocompParams->globalSize[0] = N*globalMPISize; // assumes outermost dimension gets expanded by each rank  

#ifndef NDEBUG
	printf("size definitions\n"); 
#endif

	iocompParams->localDataSize = 1; 
	iocompParams->globalDataSize = 1; 
	
	for (int i = 0; i < iocompParams->NDIM; i++)
	{
		iocompParams->localDataSize *= iocompParams->localSize[i]; 
	} 

	for (int i = 0; i < iocompParams->NDIM; i++)
	{
		iocompParams->globalDataSize *= iocompParams->globalSize[i]; 
	} 

	double* data = NULL; 
	data = (double*)malloc(iocompParams->localDataSize*sizeof(double)); //send array is divided by numnber of compute ranks 

	for(i = 0; i < iocompParams->localDataSize; i++)
	{
		data[i] =iocompParams->localDataSize * globalMPIRank + i; 
	}
	return data; 
} 
