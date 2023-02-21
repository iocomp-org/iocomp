#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"
#define filename "write_time.csv"

int getPair(struct iocomp_params *iocompParams)
{
	/*
	 * ioServer is pairing up with the lower half.
	 * if total size is 8, and ioRank is 4 in globalComm, its pairing with 0 rank of globalComm. 
	 */ 
	int ierr; 
	int globalRank, globalSize; 

	ierr = MPI_Comm_rank(iocompParams->globalComm, &globalRank); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_size(iocompParams->globalComm, &globalSize); 
	mpi_error_check(ierr);

	if(iocompParams->colour == ioColour) 
	{

		int source; 
		if(globalSize > NODESIZE*2)
		{
			source = globalRank - NODESIZE; 
		}
		else
		{
			source = globalRank - globalSize/2; 
		} 
		return(source); 
	}

	/*
	 * compServer is pairing up with the upper half of ranks. 
	 * if total size is 8, and compRank is 0 in globalComm, its pairing with 4th rank of globalComm. 
	 */ 
	else if(iocompParams->colour == compColour) 
	{
		int dest; 

		if(globalSize > NODESIZE*2)
		{
			dest = globalRank + NODESIZE; 
		}
		else
		{
			dest = globalRank + globalSize/2; 
		} 
		return(dest); 
	}

	else
	{
		printf("Invalid assignment of colours for ranks. Exiting \n"); 
		exit(0); 
	}

}

