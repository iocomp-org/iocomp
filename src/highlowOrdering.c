#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

#define ioColour 0 
#define compColour 1 

/*
 * Assigns io colour based on high low ordering 
 * Comp server ranks are lower ranked, IO server ranks are higher ranked 
 * in 1 node when using Hyperthreads 
 * After 1 node the logic operates in a step function way 
 * and divides ranks into nodes given by NODESIZE
 * For ex: for archer2, this should be the division given for 4nodes with
 * nodesize 128
 * NODE 1 0		- 127		: compute 
 * NODE 1 128 - 255		: I/O
 * NODE 2 256 - 383		: compute 
 * NODE 2 384	- 511 	: I/O
 * NODE 3 512	-	639 	: compute
 * NODE 3 640	-	767 	: I/O
 * NODE 4 768	-	895 	: compute
 * NODE 4 896	-	1023	: I/O
 */

void highlowOrdering(struct iocomp_params *iocompParams) 
{
	int globalRank, globalSize; 
	char nodeName[MPI_MAX_PROCESSOR_NAME];
	int namelen; 
	int ierr; 
	ierr = MPI_Comm_rank(iocompParams->globalComm, &globalRank); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_size(iocompParams->globalComm, &globalSize); 
	mpi_error_check(ierr); 
	ierr = MPI_Get_processor_name (nodeName, &namelen);
	mpi_error_check(ierr); 

	int numNodes = (int)(globalSize/(iocompParams->NODESIZE*2)); 
	int lastwholeNode = numNodes *iocompParams->NODESIZE*2; 
	int currentNode, nodeRank; 

	if(globalRank < lastwholeNode) // globalRank comes within full node?
	{
		currentNode = (int)( globalRank/(iocompParams->NODESIZE*2) );
		nodeRank = globalRank - currentNode*iocompParams->NODESIZE*2;
		if(nodeRank < iocompParams->NODESIZE)
		{
			iocompParams->colour = compColour; 
		}
		else
		{
			iocompParams->colour = ioColour; 
		}
	}

	else if(globalRank >= lastwholeNode)// globalRank is not within full node.
	{
		if(globalRank  < (globalSize + lastwholeNode)/2)
		{
			iocompParams->colour = compColour; 
		}
		else
		{
			iocompParams->colour = ioColour; 
		}
	}

	// check if colour is compColour or ioColour. 
	assert(iocompParams->colour == 0 || iocompParams->colour == 1); 
#ifdef VERBOSE
	fprintf(iocompParams->debug,"highLowOrdering -> ranks assigned their colours %i with global rank %i \n", iocompParams->colour, globalRank); 
#endif
} 

