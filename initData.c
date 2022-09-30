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
	ierr = MPI_Comm_size(iocompParams->globalComm, &globalMPISize); 
	mpi_error_check(ierr);

#ifndef NDEBUG
	printf("MPI ranks and sizes\n"); 
#endif
	
	/*
	 * Array size specs initialise
	 */ 
	iocompParams->localSize = malloc(sizeof(int)*NDIM_define);
	iocompParams->globalSize = malloc(sizeof(int)*NDIM_define);
	iocompParams->NDIM = NDIM_define;// number of dimensions  

	for (int i = 0; i < iocompParams->NDIM; i++)
	{
		iocompParams->localSize[i] = N; 
		iocompParams->globalSize[i] = N; 
	}
	iocompParams->globalSize[0] = N*globalMPISize; // assumes outermost dimension gets expanded by each rank  
#ifndef NDEBUG
	printf("localSize, globalSize, arrayStart initialised\n"); 
#endif

	/*
	 * Array local, global sizes initialise 
	 */ 
	iocompParams->localDataSize = 1; 
	iocompParams->globalDataSize = 1; 
	for (i = 0; i < iocompParams->NDIM; i++)
	{
		iocompParams->localDataSize *= iocompParams->localSize[i]; 
		iocompParams->globalDataSize *= iocompParams->globalSize[i]; 
	} 
#ifndef NDEBUG
	printf("size definitions\n"); 
#endif

	double* data = NULL; 
	data = (double*)malloc(iocompParams->localDataSize*sizeof(double)); //send array is divided by numnber of compute ranks 

	for(i = 0; i < iocompParams->localDataSize; i++)
	{
		data[i] =iocompParams->localDataSize * globalMPIRank + i; 
	}

#ifndef NDEBUG
	/*
	* Testing * by printing 
	*/ 
	printf("MPI Size %i  and Rank  %i \n", globalMPISize, globalMPIRank); 
	printf("printing localsize and globalsize \n"); 
	for (int i = 0; i < iocompParams->NDIM; i++)
	{
		printf("%i %i \n", iocompParams->localSize[i], iocompParams->globalSize[i]); 
	}
	printf("printing localdatasize %i and globaldatasize %i \n", iocompParams->localDataSize, iocompParams->globalDataSize  ); 
	for(i = 0; i < iocompParams->localDataSize; i++)
	{
		printf("%lf ",data[i]); 
	}
	printf("\n"); 
#endif

	return data; 
} 
