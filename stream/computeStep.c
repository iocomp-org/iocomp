/* Inspired from the STREAM benchmark */ 
/* github url https://github.com/jeffhammond/STREAM.git */ 

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"


void computeStep(struct iocomp_params *iocompParams, struct stream_params *streamParams, MPI_Comm comm)
{
#ifndef NDEBUG
	printf("stream -> Starting computeStep \n"); 
#endif

	/*
	 * mallocing the pointers 
	 */ 
	double* a = NULL; // initialise data pointer  
	double* b = NULL; // initialise data pointer  
	double* c = NULL; // initialise data pointer  
	a = (double*)malloc(streamParams->localDataSize*sizeof(double)); // one rank only sends to one rank
	b = (double*)malloc(streamParams->localDataSize*sizeof(double)); // one rank only sends to one rank
	c = (double*)malloc(streamParams->localDataSize*sizeof(double)); // one rank only sends to one rank
	a = init(streamParams,a); // initialise a array

	double wallTime_start, wallTime_end; 
	wallTime_start = MPI_Wtime(); 

	/*
	 * Initialise the timers 
	 */ 
	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < LOOPCOUNT; j ++)
		{	
			streamParams->compTimer[i][j] = 0.0; 
			streamParams->waitTimer[i][j] = 0.0; 
			streamParams->sendTimer[i][j] = 0.0; 
		}	
	}


	/*
	 * STREAM kernels, add, copy, scale and triad 
	 * loop till LOOPCOUNT to get an average 
	 */
	for(int k = 0; k< LOOPCOUNT; k++) // averaging 
	{
#ifndef NDEBUG
		printf("stream -> stream loop starts\n"); 
#endif
		if(k>0) { add_wait(iocompParams, streamParams, k);} // wait for copy to be send its data 

		/*
		 * COPY
		 */ 
		copy(iocompParams, streamParams, k, c, a); // send copy data and get timers for send and compute  
		if(k>0) { scale_wait(iocompParams, streamParams, k);} // wait for send to be finished sending its data  

		/*
		 * SCALE
		 */ 
		scale(iocompParams, streamParams, k, c, b); // send scale data and get timers for send and compute  
		copy_wait(iocompParams, streamParams, k); // wait for copy to be send its data 

		/*
		 * ADD
		 */ 
		add(iocompParams, streamParams, k, c, a, b); // send copy data and get timers for send and compute  
		if(k>0){triad_wait(iocompParams, streamParams, k);}  // wait for copy to be send its data 

		/*
		 * TRIAD 
		 */ 
		triad(iocompParams, streamParams, k, c, a, b); // send copy data and get timers for send and compute  

	} // end avg loop  

	stopSend(iocompParams); // send ghost message to stop MPI_Recvs 
#ifndef NDEBUG
	printf("After stopSend function\n"); 
#endif

	wallTime_end = MPI_Wtime(); 
	streamParams->wallTimer=wallTime_start - wallTime_end; 

	free(a); 
	free(b); 
	free(c); 
	a = NULL; 
	b = NULL; 
	c = NULL; 
} 



