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

	int myrank, mysize;  
	MPI_Comm_rank(comm, &myrank); 
	MPI_Comm_size(comm, &mysize); 
	streamParams->globalDataSize = mysize * streamParams->localDataSize; 

	/*
	 * Initialise the timers 
	 */ 
	for(int i = 0; i < KERNELS; i++)
	{
		int writeCounter = 0; 
		for(int j = 0; j < AVGLOOPCOUNT; j ++)
		{	
			streamParams->compTimer[i][j] = 0.0; 
			streamParams->waitTimer[i][j] = 0.0; 
			streamParams->sendTimer[i][j] = 0.0; 
		}	
	}

	/*
	 * STREAM kernels ADD, COPY, SCALE and TRIAD
	 * loop till AVGLOOPCOUNT to get an average 
	 */
	int k; 
	for(k = 0; k< AVGLOOPCOUNT; k++) // averaging 
	{
#ifndef NDEBUG
		printf("stream -> stream loop starts\n"); 
#endif

		/*
		 * COPY(C) + MPITEST(A)
		 * WAIT(A)
		 * SEND(C) 
		 */ 
		copy(iocompParams, streamParams, k, c, a); // loop
		if(k > 0) // wait for at least 1 iteration before A gets sent
		{
			triad_wait(iocompParams, streamParams, k-1);
		} 
		copy_send(iocompParams, streamParams, k,c);

		/*
		 * SCALE(B) + MPITEST(C)
		 * WAIT(C)
		 * SEND(B)
		 */ 
		scale(iocompParams, streamParams, k, c, b);
		copy_wait(iocompParams, streamParams, k);
		scale_send(iocompParams, streamParams, k, b );

		/*
		 * ADD(C) + MPITEST(B)
		 * WAIT(B) 
		 * SEND(C) 
		 */ 
		add(iocompParams, streamParams, k, c, a, b);
		scale_wait(iocompParams, streamParams, k);
		add_send(iocompParams, streamParams, k, c );

		/*
		 * TRIAD(A) + MPITEST(C)
		 * WAIT(C)
		 * SEND(A)
		 */ 
		triad(iocompParams, streamParams, k, c, a, b);
		add_wait(iocompParams, streamParams, k);
		triad_send(iocompParams, streamParams, k, a);
	} // end avg loop 

	triad_wait(iocompParams, streamParams, k-1); // catch any triad sending after loop ends 
#ifdef TEST_VALS
	if(!myrank)
	{
		test_vals(iocompParams, streamParams, a, "TRIAD"); 	
	} 
#endif 

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



