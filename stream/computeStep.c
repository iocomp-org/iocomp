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
	//int mpiWaitFlag[4] ; // mpiwaits array

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
	int k; 
	for(k = 0; k< LOOPCOUNT; k++) // averaging 
	{
#ifndef NDEBUG
		printf("stream -> stream loop starts\n"); 
#endif

		/*
		 * COPY
		 */ 
		if(k>0) { add_wait(iocompParams, streamParams, k-1);} // wait for C array to be sent, only after the first iteration of add wait 
		copy(iocompParams, streamParams, k, c, a); // send copy data and get timers for send and compute  
//		if(k>0){mpiWaitFlag[TRIAD] = dataSendTest(iocompParams,&streamParams->requestArray[TRIAD]);} // test if TRIAD data got sent

		/*
		 * SCALE
		 */ 
		if(k>0) { scale_wait(iocompParams, streamParams, k-1);} // wait for B to be sent 
		scale(iocompParams, streamParams, k, c, b); // send scale data and get timers for send and compute  
//		mpiWaitFlag[COPY]=dataSendTest(iocompParams,&streamParams->requestArray[COPY]); // test if COPY data got sent 

		/*
		 * ADD
		 */ 
		copy_wait(iocompParams, streamParams, k); // wait for C to be sent  
		add(iocompParams, streamParams, k, c, a, b); // send add data and get timers for send and compute   
//		mpiWaitFlag[SCALE]=dataSendTest(iocompParams,&streamParams->requestArray[SCALE]); // test if SCALE data got sent  

		/*
		 * TRIAD 
		 */ 
		if(k>0){triad_wait(iocompParams, streamParams, k-1);}  // wait for A to be sent 
		triad(iocompParams, streamParams, k, c, a, b); // send copy data and get timers for send and compute  
//		mpiWaitFlag[ADD]=dataSendTest(iocompParams,&streamParams->requestArray[ADD]); // test if ADD data got sent  
	
	} // end avg loop  
	
	/*
	 * for the last iteration of K, wait for all data to be sent. 
	 */ 
	if(k>0) { add_wait(iocompParams, streamParams, k-1);} // wait for C array to be sent, only after the first iteration of add wait 
	if(k>0) { scale_wait(iocompParams, streamParams, k-1);} // wait for B to be sent 
	if(k>0)	{	triad_wait(iocompParams, streamParams, k-1);}  // wait for A to be sent 

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



