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
		int writeCounter = 0; 
		for(int j = 0; j < LOOPCOUNT; j ++)
		{	
			streamParams->compTimer[i][j] = 0.0; 
			streamParams->waitTimer[i][j] = 0.0; 
			if(!j%streamParams->writeFreq){ // as write timer will have a different range 
				streamParams->sendTimer[i][writeCounter] = 0.0; 
				writeCounter++; 
			} 
		}	
	}

	/*
	 * STREAM kernels ADD, COPY, SCALE and TRIAD
	 * loop till LOOPCOUNT to get an average 
	 */
	int k; 
	for(k = 0; k< LOOPCOUNT; k++) // averaging 
	{
#ifndef NDEBUG
		printf("stream -> stream loop starts\n"); 
#endif

		/*
		 * Wait for ADD(C) 
		 * COPY(C) send
		 * MPI Test for SCALE(B) and TRIAD(A)
		 */ 
		if(k>0) { add_wait(iocompParams, streamParams, k-1);}
		copy(iocompParams, streamParams, k, c, a); 

		/*
		 * Wait for SCALE(B) 
		 * SCALE(B) send 
		 * MPI Test for COPY(C) and TRIAD(A)
		 */ 
		if(k>0) { scale_wait(iocompParams, streamParams, k-1);} 
		scale(iocompParams, streamParams, k, c, b);

		/*
		 * Wait for COPY(C) 
		 * ADD(C) send 
		 * MPI Test for SCALE(B) and TRIAD(A)
		 */ 
		copy_wait(iocompParams, streamParams, k);
		add(iocompParams, streamParams, k, c, a, b);

		/*
		 * Wait for TRIAD(A) 
		 * TRIAD(A) send  
		 * MPI Test for ADD(C) and SCALE(B)
		 */ 
		if(k>0){triad_wait(iocompParams, streamParams, k-1);} 
		triad(iocompParams, streamParams, k, c, a, b);
	
	} // end avg loop  
	
	/*
	 * for the last iteration of K, wait for all data to be sent. 
	 */ 
	if(k>0) { 
		add_wait(iocompParams, streamParams, k-1); 
		scale_wait(iocompParams, streamParams, k-1);
		triad_wait(iocompParams, streamParams, k-1); 
	} 

	stopSend(iocompParams); // send ghost message to stop MPI_Recvs 
#ifndef NDEBUG
	printf("After stopSend function\n"); 
#endif

	wallTime_end = MPI_Wtime(); 
	streamParams->wallTimer=wallTime_end - wallTime_start; 

	free(a); 
	free(b); 
	free(c); 
	a = NULL; 
	b = NULL; 
	c = NULL; 
} 



