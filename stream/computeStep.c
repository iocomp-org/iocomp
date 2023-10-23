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
	fprintf(iocompParams->debug, "stream-> starting compute step  \n");
#endif

	/*
	 * mallocing the pointers 
	 */ 
	double* b = NULL; // initialise data pointer  
	double* c = NULL; // initialise data pointer  
	double* a = NULL; // initialise data pointer  

	/*
	 * Initialise a, b, c. 
	 * Initialise shared windows associated with arrays *if* shared flag is true
	 * otherwise return the malloced arrays with localDataSize 
	 */ 
	winInits(iocompParams, streamParams->localDataSize); 

	a = iocompParams->array[2];
	c = iocompParams->array[1];
	b = iocompParams->array[0];
	// b = iocompParams->array[0];

	// tempororary mallocing of c for testing with scale array 
	//c = (double*)malloc(streamParams->localDataSize*sizeof(double)); // one rank only sends to one rank
	//malloc_check(c); 
	//a = (double*)malloc(streamParams->localDataSize*sizeof(double)); // one rank only sends to one rank
	//malloc_check(a); 

#ifndef NDEBUG
	fprintf(iocompParams->debug, "stream->arrays malloced \n");
#endif

	for(int i = 0; i < streamParams->localDataSize; i++)
	{
		a[i] = 1.0; 
		b[i] = 2.0; 
		c[i] = 0.0; 
	}

	double wallTime_start, wallTime_end; 
	wallTime_start = MPI_Wtime(); 

	int myrank, mysize;  
	char fileWrite_ADD[10] = ""; 
	char fileWrite_SCALE[10] = ""; 
	char fileWrite_TRIAD[10] = ""; 
	//MPI_Comm_rank(comm, &myrank); 
	//MPI_Comm_size(comm, &mysize); 
	//streamParams->globalDataSize = mysize * streamParams->localDataSize; 

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
	 * and send data for every multiple of WRITE_FREQ
	 */
	int iter; 
	for(iter = 0; iter< AVGLOOPCOUNT; iter++) // averaging 
	{
#ifndef NDEBUG
		fprintf(iocompParams->debug, "stream-> stream loop starts \n");
#endif

		/*
		 * COPY(C) + MPITEST(A)
		 * WAIT(A)
		 * SEND(C) 
		 */ 
		// copy(iocompParams, streamParams, iter, c, a); // loop
		//if( (iter+1)%WRITE_FREQ == 0 && iter > 0) 
		//{
		//	// wait for at least one iteration of TRIAD before waiting for TRIAD
		//	triad_wait(iocompParams, streamParams, iter-1);
		//} 
		//if(iter%WRITE_FREQ==0)
		//{
		//	copy_send(iocompParams, streamParams, iter,c);
		//} 

		/*
		 * WAIT(B)
		 * SCALE(B) 
		 * SEND(B)
		 */ 
		if(iter > 0)
		{
			printf("file write before scale wait %s \n", fileWrite_SCALE); 
			scale_wait(iocompParams, streamParams, iter, b, fileWrite_SCALE); 
			winActivateInfo(iocompParams, b); 
			winTestInfo(iocompParams, c); 
			winTestInfo(iocompParams, a); 
		}
		else
		{
			winActivateInfo(iocompParams, b); 
		}
		snprintf(fileWrite_SCALE, sizeof(fileWrite_SCALE), "B_%i",iter);
		preDataSend(iocompParams, b, fileWrite_SCALE); 
		scale(iocompParams, streamParams, iter, c, b);
		scale_send(iocompParams, streamParams, iter, b);

		/*
		 * WAIT(C) 
		 * ADD(C)
		 * SEND(C) 
		 */ 
		if(iter > 0)
		{
			printf("file write before add wait %s \n", fileWrite_ADD); 
			add_wait(iocompParams, streamParams, iter, c, fileWrite_ADD); 
			winActivateInfo(iocompParams, c); 
			winTestInfo(iocompParams, a); 
			winTestInfo(iocompParams, b); 
		}
		else
		{
			winActivateInfo(iocompParams, c); 
			winTestInfo(iocompParams, b); 
		}
		snprintf(fileWrite_ADD, sizeof(fileWrite_ADD), "C_%i",iter);
		preDataSend(iocompParams, c, fileWrite_ADD); 
		add(iocompParams, streamParams, iter, c, a, b);
		add_send(iocompParams, streamParams, iter, c);

		/*
		 * WAIT(A)
		 * TRIAD(A)
		 * SEND(A)
		 */ 
		if(iter > 0)
		{
			printf("file write before triad wait %s \n", fileWrite_TRIAD); 
			triad_wait(iocompParams, streamParams, iter, a, fileWrite_TRIAD); 
			winActivateInfo(iocompParams, a); 
			winTestInfo(iocompParams, b); 
			winTestInfo(iocompParams, c); 
		}
		else
		{
			winActivateInfo(iocompParams, a); 
			winTestInfo(iocompParams, b); 
			winTestInfo(iocompParams, c); 
		}
		snprintf(fileWrite_TRIAD, sizeof(fileWrite_TRIAD), "A_%i",iter);
		preDataSend(iocompParams, a, fileWrite_TRIAD); 
		triad(iocompParams, streamParams, iter, c, a, b);
		triad_send(iocompParams, streamParams, iter, a);

	} // end avg loop 
#ifndef NDEBUG
	fprintf(iocompParams->debug, "stream-> after end of avg loop\n");
#endif

	if(streamParams->HT_flag)
	{
//		iter--; // iter has incremented value
//		snprintf(fileWrite, sizeof(fileWrite), "B_%i",iter);
//		printf("file write before scale wait %s \n", fileWrite); 
		scale_wait(iocompParams, streamParams, iter, b, fileWrite_SCALE); 
//		snprintf(fileWrite, sizeof(fileWrite), "C_%i",iter);
//		printf("file write before add wait %s \n", fileWrite); 
		add_wait(iocompParams, streamParams, iter, c, fileWrite_ADD); 
//		snprintf(fileWrite, sizeof(fileWrite), "A_%i",iter);
//		printf("file write before triad wait %s \n", fileWrite); 
		triad_wait(iocompParams, streamParams, iter, a, fileWrite_TRIAD); 
	} 
	stopSend(iocompParams); // send ghost message to stop MPI_Recvs and post win free for shared windows 
#ifndef NDEBUG
	fprintf(iocompParams->debug, "stream->data send complete\n");
#endif

	winFinalise(iocompParams); // finalise arrays 
	wallTime_end = MPI_Wtime(); 
	assert(wallTime_end!=wallTime_start); 
	streamParams->wallTimer=wallTime_end - wallTime_start; 

#ifndef NDEBUG
	fprintf(iocompParams->debug, "stream->compute step completed\n");
#endif
} 



