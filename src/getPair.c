#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

/*
 * PRINT_ORDERING flag prints the node id and the core id of different 
 * processes 
 */ 
int sched_getcpu();


int getPair(struct iocomp_params *iocompParams)
{
	/*
	 * ioServer is pairing up with the lower half.
	 * if total size is 8, and ioRank is 4 in globalComm, its pairing with 0 rank of globalComm. 
	 */ 
	int ierr; 
	int globalRank, globalSize; 
  char nodeName[MPI_MAX_PROCESSOR_NAME];
	int namelen; 

	ierr = MPI_Comm_rank(iocompParams->globalComm, &globalRank); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_size(iocompParams->globalComm, &globalSize); 
	mpi_error_check(ierr);
  MPI_Get_processor_name (nodeName, &namelen);

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
#ifdef PRINT_ORDERING
		if(!iocompParams->pairPrintCounter) // only print message first time? 
		{
			printf("IO colour rank %i out of %i size on cpu_id %02d of node %s gets data from COMP rank %i\n", 
				globalRank, globalSize,sched_getcpu(),nodeName, source); 
			iocompParams->pairPrintCounter = 1; 
		} 
#endif
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
#ifdef PRINT_ORDERING
		if(!iocompParams->pairPrintCounter) // only print message first time? 
			{
				printf("COMP colour rank %i out of %i size on cpu_id %02d of node %s gets data from COMP rank %i\n", 
					globalRank, globalSize,sched_getcpu(),nodeName, dest); 
				iocompParams->pairPrintCounter = 1; 
			} 
#endif
		return(dest); 
	}

	else
	{
		printf("Invalid assignment of colours for ranks. Exiting \n"); 
		exit(0); 
	}

}

