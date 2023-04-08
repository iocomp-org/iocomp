#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

#define ioColour 0 
#define compColour 1 
#define HIGH_LOW 1 

#ifndef MAXSPACE
#define MAXSPACE 20 // to set space limit for printing ordering statements 
#endif

/*
 * Divides the particular rank into comp server and io server 
 */

void comm_split(struct iocomp_params *iocompParams, MPI_Comm comm)
{

#ifndef NDEBUG
		printf("commSplit -> Start with flag value %i \n", iocompParams->hyperthreadFlag); 
#endif

	int ierr = MPI_Comm_dup(comm, &iocompParams->globalComm); // comm is assigned to globalComm
	mpi_error_check(ierr); 
	int globalRank; 
	MPI_Comm_rank(iocompParams->globalComm, &globalRank); 

#ifndef NDEBUG
		printf("commSplit -> comm duplicate \n"); 
#endif
	
	// header file for printing ordering 
#ifdef PRINT_ORDERING
		if(!globalRank) // header file by global rank 0 
		{
			printf("%*s | %*s | %*s | %*s | %*s | %*s \n", 10, "TYPE", MAXSPACE, "MPI RANK", 
				MAXSPACE, "MPI SIZE", MAXSPACE,"CPU-ID", MAXSPACE, "NODE-ID", MAXSPACE, "PAIR");
		} 
#endif

	if(iocompParams->hyperthreadFlag) // check if flag is true? 
	{
		int ordering; // defines how IO and compute threads are going to organised 
		ordering = HIGH_LOW; 
		int globalRank, globalSize; 
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
		 * in 1 node when using Hyperthreads 
		 */ 
		if(ordering == HIGH_LOW)
		{	   
      highlowOrdering(iocompParams); 
#ifdef PRINT_ORDERING
			iocompParams->pairPrintCounter = 0; // so that io and comm processes dont print too many messages. 
#endif 
		}
#ifndef NDEBUG
		printf("commSplit -> colour assigned based on HIGH LOW ordering \n"); 
#endif

		/*
		 * split MPI comm
		 */
		ierr =	MPI_Comm_split(iocompParams->globalComm, iocompParams->colour, globalRank,	&splitComm);  // splitcommunicator based on colour 
		mpi_error_check(ierr);
#ifndef NDEBUG
		printf("commSplit -> MPI comm split \n"); 
#endif
		
		if(iocompParams->colour == compColour)
		{
			ierr = MPI_Comm_dup(splitComm, &iocompParams->compServerComm); // compute communicator for compute tasks and colour != 0 
			mpi_error_check(ierr); 
#ifndef NDEBUG
		printf("commSplit -> MPI compServerComm initialised \n"); 
#endif
		} 

		else
		{
			ierr = MPI_Comm_dup(splitComm, &iocompParams->ioServerComm); // compute communicator for compute tasks and colour != 0 
			mpi_error_check(ierr); 
#ifndef NDEBUG
		printf("commSplit -> MPI ioServerComm initialised \n"); 
#endif


		} 
	

#ifndef NDEBUG
		int ioRank, computeRank; 
		if ( iocompParams->colour == ioColour )
		{
			MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
			printf("commSplit -> Hello from ioServeComm with rank %i and colour %i \n", ioRank, iocompParams->colour); 
		} 
		else if ( iocompParams->colour == compColour )
		{
			MPI_Comm_rank(iocompParams->compServerComm, &computeRank); 
			printf("commSplit -> Hello from computecomm with rank %i and colour %i \n", computeRank, iocompParams->colour); 
		} 
#endif
	}

	
	else // if HT flag is false 
	{
		ierr = MPI_Comm_dup(iocompParams->globalComm, &iocompParams->compServerComm); // compute communicator for compute tasks and colour != 0 
		mpi_error_check(ierr); 
		ierr = MPI_Comm_dup(iocompParams->globalComm, &iocompParams->ioServerComm); // compute communicator for compute tasks and colour != 0 
		mpi_error_check(ierr); 
#ifndef NDEBUG
		printf("commSplit -> MPI ioServerComm and compServerComm set to globalComm for hyperthreads switched off\n"); 
#endif
	}
}

