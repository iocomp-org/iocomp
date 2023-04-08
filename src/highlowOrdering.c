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
 */

/*
 * PRINT_ORDERING flag prints the node id and the core id of different 
 * processes 
 */ 
int sched_getcpu();

void highlowOrdering(struct iocomp_params *iocompParams) 
{
	int globalRank, globalSize; 
  char nodeName[MPI_MAX_PROCESSOR_NAME];
	int namelen; 
	MPI_Comm_rank(iocompParams->globalComm, &globalRank); 
	MPI_Comm_size(iocompParams->globalComm, &globalSize); 
  MPI_Get_processor_name (nodeName, &namelen);

	int numNodes = (int)(globalSize/(NODESIZE*2)); 
	int lastwholeNode = numNodes *NODESIZE*2; 

	if(globalSize <= NODESIZE*2)
	{
		if(globalRank < globalSize/2){
			iocompParams->colour = compColour; 
		} 
		else{
			iocompParams->colour = ioColour; 
		}
	} 

	// size greater than 256 
	else{           

		if(globalRank < lastwholeNode){ // globalRank comes within full node? 
			for(int x = 0; x <= numNodes; x+=2 ) 
			{
				if(globalRank <  (x+1)*NODESIZE && globalRank >= (x)*NODESIZE){
					iocompParams->colour = compColour; 
				} 
				if(globalRank >=  (x+1)*NODESIZE && globalRank < (x+2)*NODESIZE){
					iocompParams->colour = ioColour; 
				} 
			}
		} 

		else if(globalRank >= lastwholeNode){ // globalRank is not within full node. 
			if(globalRank  < (globalSize + lastwholeNode)/2){
				iocompParams->colour = compColour; 
			} 
			else{
				iocompParams->colour = ioColour; 
			} 

		}
	} 
} 

