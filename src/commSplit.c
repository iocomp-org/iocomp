#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

#define ioColour 0 
#define compColour 1 
#define HIGH_LOW 1 

void comm_split(struct iocomp_params *iocompParams, MPI_Comm comm)
{
	/*
	 * Divides the particular rank into comp server and io server 
	 */

	int ordering; // defines how IO and compute threads are going to organised 
	ordering = HIGH_LOW; 
	
	int globalRank, globalSize, ierr, i;
	ierr = MPI_Comm_dup(comm, &iocompParams->globalComm); // comm is assigned to globalComm
	mpi_error_check(ierr); 
	MPI_Comm splitComm; 
	ierr = MPI_Comm_rank(iocompParams->globalComm, &globalRank); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_size(comm, &globalSize); 
	mpi_error_check(ierr); 
	
	/*
	 * Check if there are evenly matched IO Servers and Comp Servers 
	 */ 

	if (globalSize %2 != 0 || globalSize < 2)  
	{
		printf("Invalid globalSize. It needs to be an even number and greater than 1 \n"); 
		exit(1); 
	} 

	/*
	 * assume high low ordering of hyperthreads 
	 * Comp server ranks are lower ranked, IO server ranks are higher ranked 
	 */ 

	if(ordering == HIGH_LOW) // if size is lesser than fullNode of archer2 
	{	   
		if (globalRank < globalSize/2) // comp server 
		{
			iocompParams->colour					= compColour;
			//iocompParams->compServerRank	= globalRank;
			//iocompParams->ioServerRank		= globalSize - globalRank - 1;  
			//iocompParams->compServerSize  = globalSize/2; 
			//iocompParams->ioServerSize    = globalSize/2;
		} 

		else if(globalRank >= globalSize/2) // io server
		{
			iocompParams->colour					= ioColour; 
			//iocompParams->ioServerRank		= globalRank;
			//iocompParams->compServerRank	= globalSize - globalRank - 1;  
			//iocompParams->compServerSize	= globalSize/2; 
			//iocompParams->ioServerSize    = globalSize/2;
		}
	}

	/*
	 * split MPI comm
	 */

	ierr =	MPI_Comm_split(iocompParams->globalComm, iocompParams->colour, globalRank,	&splitComm);  // splitcommunicator based on colour 
	mpi_error_check(ierr);

#ifndef NDEBUG
	printf("MPI comm split \n"); 
#endif

	ierr = MPI_Comm_dup(splitComm, &iocompParams->compServerComm); // compute communicator for compute tasks and colour != 0 
	mpi_error_check(ierr); 

#ifndef NDEBUG
	printf("MPI compServerComm initialised \n"); 
#endif

	ierr = MPI_Comm_dup(splitComm, &iocompParams->ioServerComm); // compute communicator for compute tasks and colour != 0 
	mpi_error_check(ierr); 

#ifndef NDEBUG
	printf("MPI ioServerComm initialised \n"); 
#endif
}
