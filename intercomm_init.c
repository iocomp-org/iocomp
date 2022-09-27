#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

#define compColour 1
#define ioColour 0
#define fullNode 256 

/*
* Initialises the iocomp_params struct 
* Assumes that the high and low distribution of threads 
* Divides the rank into comp server and io server 
*/

void intercomm_init(struct iocomp_params *iocompParams, MPI_Comm comm)
{
	
#ifndef NDEBUG
			printf("Start of intercomm_init\n"); 
#endif

	int globalRank, globalSize; 
	MPI_Comm_rank(comm, &globalRank);
	MPI_Comm_size(comm, &globalSize); 
	
	if (globalSize %2 != 0 || globalSize < 2) // exceptions 
	{
		printf("Invalid globalSize. It needs to be an even number and greater than 1 \n"); 
		exit(1); 
	}
	
	/*
	* assume high low ordering of hyperthreads 
	*/ 
	if(globalSize < fullNode) // if size is lesser than fullNode of archer2 
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
	* Assumes io server rank is always 0
	*/ 
	//	if (globalRank != 0) // comp server 
	//	{
	//		iocompParams->colour					= compColour;
	//		iocompParams->compServerRank	= globalRank;
	//		iocompParams->ioServerRank		= 0;  
	//		iocompParams->compServerSize	= globalSize - 1; 
	//	} 
	//	else if(globalRank == 0) // io server
	//	{
	//		iocompParams->colour					= ioColour; 
	//		iocompParams->ioServerRank		= 0;
	//		iocompParams->compServerRank	= globalRank; 
	//		iocompParams->ioServerSize		= 1;
	//		iocompParams->compServerSize	= globalSize - 1; 
	//	}
	

#ifndef NDEBUG
			printf("global rank  %i colour %i ioServerRank %i compServerRank %i \n",  globalRank,  iocompParams->colour,  iocompParams->ioServerRank,  iocompParams->compServerRank); 
			printf("End intercomm_init\n"); 
#endif

} 

