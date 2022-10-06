#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

#define compColour 1
#define ioColour 0
#define fullNode 256 
#define HIGH_LOW 1
#define FIXED_IO_RANK 0

/*
 * Initialises the iocomp_params struct 
 * Divides the particular rank into comp server and io server 
 */

void intercommInit(struct iocomp_params *iocompParams, MPI_Comm comm, int NDIM, int* localArraySize)
{

#ifndef NDEBUG
	printf("Start of intercomm_init\n"); 
#endif

	int ordering; // defines how IO and compute threads are going to organised 
	ordering = HIGH_LOW; 

	int globalRank, globalSize, ierr, i;
	MPI_Comm_rank(comm, &globalRank);
	MPI_Comm_size(comm, &globalSize); 
	ierr =MPI_Comm_dup(comm, &iocompParams->globalComm); 
	mpi_error_check(ierr); 

	/*
	 * Check if there are evenly matched IO Servers and Comp Servers 
	 */ 

	if (globalSize %2 != 0 || globalSize < 2)  
	{
		printf("Invalid globalSize. It needs to be an even number and greater than 1 \n"); 
		exit(1); 
	} 

	if(globalRank == 0)
	{
		printf("Program starts with size %i \n",  globalSize); 
	} 

	/*
	 * Array size specs initialise
	 */ 

	iocompParams->localSize = malloc(sizeof(int)*NDIM);
	iocompParams->globalSize = malloc(sizeof(int)*NDIM);
	iocompParams->NDIM = NDIM;// number of dimensions  

	for (i = 0; i < iocompParams->NDIM; i++)
	{
		iocompParams->localSize[i] = localArraySize[i]; // initialise based on passed argument  
		iocompParams->globalSize[i] = localArraySize[i]; 
	}
	iocompParams->globalSize[0]*= globalSize; // assumes outermost dimension gets expanded by each rank  
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
	printf("size definitions\n"); 
#endif

	/*
	 * assume high low ordering of hyperthreads 
	 * Comp server ranks are lower ranked, IO server ranks are higher ranked 
	 */ 

	if(ordering == HIGH_LOW) // if size is lesser than fullNode of archer2 
	{	   
		if (globalRank < globalSize/2) // comp server 
		{
			iocompParams->colour					= compColour;
			iocompParams->compServerRank	= globalRank;
			iocompParams->ioServerRank		= globalSize - globalRank - 1;  
			iocompParams->compServerSize  = globalSize/2; 
			iocompParams->ioServerSize    = globalSize/2;
		} 

		else if(globalRank >= globalSize/2) // io server
		{
			iocompParams->colour					= ioColour; 
			iocompParams->ioServerRank		= globalRank;
			iocompParams->compServerRank	= globalSize - globalRank - 1;  
			iocompParams->compServerSize	= globalSize/2; 
			iocompParams->ioServerSize    = globalSize/2;
		}
	} 


	/*
	 * split communicators based on colour assigned in intercommInit 
	 */ 

	comm_split(iocompParams);  

	/*
	 * Intercomm created linking computeComm to globalComm via
	 * interComm
	 */ 

	intercomm_create(iocompParams); 
	
	/*
	 * Assumes io server rank is always 0
	 * Not yet implemented 
	 */ 
	//	if(ordering == FIXED_IO_RANK)
	//	{
	//		int fixedIORank; 
	//		fixedIORank = 1; 
	//		if (globalRank != fixedIORank) // comp server 
	//		{
	//			iocompParams->colour					= compColour;
	//			iocompParams->compServerRank	= globalRank;
	//			iocompParams->ioServerRank		= 0;  
	//			iocompParams->compServerSize	= globalSize - 1; 
	//		} 
	//		else if(globalRank == fixedIORank) // io server
	//		{
	//			iocompParams->colour					= ioColour; 
	//			iocompParams->ioServerRank		= 0;
	//			iocompParams->compServerRank	= globalRank; 
	//			iocompParams->ioServerSize		= 1;
	//			iocompParams->compServerSize	= globalSize - 1; 
	//		}
	//	} 


#ifndef NDEBUG
	printf("End intercomm_init\n"); 
#endif

} 

