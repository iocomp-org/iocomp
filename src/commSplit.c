/** @file commSplit.c
 *  @brief Implements splitting of MPI communicator into compute and I/O communicators. 
 *  Currently, only supports splitting using high low ordering.
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */

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

/** @brief Assigns colour to ranks based on high low ordering for splitting and shared mode. 
 * 	Then splits global communicator into compute and I/O communicators based on the colours.
 * 	For sequential mode, both ioServerComm and compServerComm are set to globalComm.
 *  @param iocompParams pointer to struct containing all parameters for the library
 *  @return void
 */
void comm_split(struct iocomp_params *iocompParams, MPI_Comm comm)
{


	int ierr = MPI_Comm_dup(comm, &iocompParams->globalComm); // comm is assigned to globalComm
	mpi_error_check(ierr); 
	int globalRank; 
	MPI_Comm_rank(iocompParams->globalComm, &globalRank); 

#ifdef VERBOSE
	fprintf(iocompParams->debug,"commSplit -> comm duplicate \n"); 
#endif

	if(iocompParams->hyperthreadFlag || iocompParams->sharedFlag) 
	{
		/* 
		 * if HT flag or shared flag is activated the IO server and Comp server are
		 * split 
		 */ 
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
		}
#ifdef VERBOSE
		fprintf(iocompParams->debug,"commSplit -> colour assigned based on HIGH LOW ordering \n"); 
#endif

		/*
		 * split MPI comm
		 */
		ierr =	MPI_Comm_split(iocompParams->globalComm, iocompParams->colour, globalRank,	&splitComm);   
		mpi_error_check(ierr);
#ifdef VERBOSE
		fprintf(iocompParams->debug,"commSplit -> MPI comm split \n"); 
#endif

		if(iocompParams->colour == compColour)
		{	
			// compute communicator for compute tasks and colour != 0
			ierr = MPI_Comm_dup(splitComm, &iocompParams->compServerComm);  
			mpi_error_check(ierr); 
#ifdef VERBOSE
			fprintf(iocompParams->debug,"commSplit -> MPI compServerComm initialised \n"); 
#endif
		} 

		else
		{
			// all other values get ioServerComm
			ierr = MPI_Comm_dup(splitComm, &iocompParams->ioServerComm); // compute communicator for compute tasks and colour != 0 
			mpi_error_check(ierr); 
#ifdef VERBOSE
			fprintf(iocompParams->debug,"commSplit -> MPI ioServerComm initialised \n"); 
#endif
		} 
	
#ifdef PRINT_ORDERING
		do_xthi(iocompParams); 	
#endif 
	}

	else // if HT flag is false 
	{
		ierr = MPI_Comm_dup(iocompParams->globalComm, &iocompParams->compServerComm); // compute communicator for compute tasks and colour != 0 
		mpi_error_check(ierr); 
		ierr = MPI_Comm_dup(iocompParams->globalComm, &iocompParams->ioServerComm); // compute communicator for compute tasks and colour != 0 
		mpi_error_check(ierr); 
#ifdef VERBOSE
		fprintf(iocompParams->debug,"commSplit -> MPI ioServerComm and compServerComm set to globalComm for hyperthreads switched off\n"); 
#endif
	}
}

