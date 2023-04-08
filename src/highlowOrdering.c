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

/* This code is adapted from an example at:
 * http://brokestream.com/procstat.html */
//int get_cpu_id()
//{
//  int i;
//  FILE* procfile = fopen("/proc/self/stat", "r");
//  long to_read = 8192;
//  char buffer[to_read];
//  int read = fread(buffer, sizeof(char), to_read, procfile);
//	if(!read)
//	{
//		printf("error"); 
//	}
//  fclose(procfile);
//
//  // Field with index 38 (zero-based counting) is the one we want
//  char* line = strtok(buffer, " ");
//  for (i = 1; i < 38; i++)
//  {
//    line = strtok(NULL, " ");
//  }
//
//  line = strtok(NULL, " ");
//  int cpu_id = atoi(line);
//  return cpu_id;
//}

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
#ifdef PRINT_ORDERING
			printf("COMP colour rank %i out of %i size on cpu_id %02d of node %s\n", 
				globalRank, globalSize,sched_getcpu(),nodeName); 
#endif
			iocompParams->colour = compColour; 
		} 
		else{
#ifdef PRINT_ORDERING
			printf("IO colour rank %i out of %i size on cpu_id %02d of node %s\n", 
				globalRank, globalSize,sched_getcpu(),nodeName); 
#endif
			iocompParams->colour = ioColour; 
		}
	} 

	// size greater than 256 
	else{           

		if(globalRank < lastwholeNode){ // globalRank comes within full node? 
			for(int x = 0; x <= numNodes; x+=2 ) 
			{
				if(globalRank <  (x+1)*NODESIZE && globalRank >= (x)*NODESIZE){
#ifdef PRINT_ORDERING
					printf("COMP colour rank %i out of %i size on cpu_id %02d of node %s\n", 
						globalRank, globalSize,sched_getcpu(),nodeName); 
#endif
					iocompParams->colour = compColour; 
				} 
				if(globalRank >=  (x+1)*NODESIZE && globalRank < (x+2)*NODESIZE){
#ifdef PRINT_ORDERING
					printf("IO colour rank %i out of %i size on cpu_id %02d of node %s\n", 
						globalRank, globalSize,sched_getcpu(),nodeName); 
#endif
					iocompParams->colour = ioColour; 
				} 
			}
		} 

		else if(globalRank >= lastwholeNode){ // globalRank is not within full node. 
			if(globalRank  < (globalSize + lastwholeNode)/2){
#ifdef PRINT_ORDERING
					printf("COMP colour rank %i out of %i size on cpu_id %02d of node %s\n", 
						globalRank, globalSize,sched_getcpu(),nodeName); 
#endif
				iocompParams->colour = compColour; 
			} 
			else{
#ifdef PRINT_ORDERING
					printf("IO colour rank %i out of %i size on cpu_id %02d of node %s\n", 
						globalRank, globalSize,sched_getcpu(),nodeName); 
#endif
				iocompParams->colour = ioColour; 
			} 

		}
	} 
} 

