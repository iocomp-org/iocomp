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

void intercommInit(struct iocomp_params *iocompParams, MPI_Comm comm)
{

#ifndef NDEBUG
	printf("Start of intercomm_init\n"); 
#endif

	int ordering; // defines how IO and compute threads are going to organised 
	ordering = HIGH_LOW; 

	int globalRank, globalSize, ierr; 
	MPI_Comm_rank(comm, &globalRank);
	MPI_Comm_size(comm, &globalSize); 
	ierr =MPI_Comm_dup(comm, &iocompParams->globalComm); 
	mpi_error_check(ierr); 
	
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

