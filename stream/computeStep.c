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
	
	/*
	 * Initialise a, b, c. 
	 * Initialise shared windows associated with arrays *if* shared flag is true
	 * otherwise return the malloced arrays with localDataSize 
	 */ 
	winInits(iocompParams, streamParams->localDataSize); 
	a = ioParams->array[0];
	c = ioParams->array[1];
	b = ioParams->array[2];

	for(int i = 0; i < streamParams->localDataSize; i++)
	{
		a[i] = 1.0; 
		b[i] = 2.0; 
		c[i] = 0.0; 
	}

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
	 * and send data for every multiple of WRITE_FREQ
	 */
	int iter; 
	for(iter = 0; iter< AVGLOOPCOUNT; iter++) // averaging 
	{
#ifndef NDEBUG
		printf("stream -> stream loop starts\n"); 
#endif

		/*
		 * COPY(C) + MPITEST(A)
		 * WAIT(A)
		 * SEND(C) 
		 */ 
		// copy(iocompParams, streamParams, iter, c, a); // loop
//		if( (iter+1)%WRITE_FREQ == 0 && iter > 0) 
//		{
//			// wait for at least one iteration of TRIAD before waiting for TRIAD
//			triad_wait(iocompParams, streamParams, iter-1);
//		} 
		//if(iter%WRITE_FREQ==0)
		//{
		//	copy_send(iocompParams, streamParams, iter,c);
		//} 

		/*
		 * SCALE(B) + MPITEST(C)
		 * WAIT(C)
		 * SEND(B)
		 */ 
		if(iter > 0)
		{
			// winWaitInfo(ioParams, b); 
			streamParams->mpiWaitFlag[SCALE]=dataSendTest(iocompParams,&streamParams->requestArray[SCALE],b); 
			dataWait(iocompParams,&streamParams->requestArray[SCALE], b);
			// streamParams->mpiWaitFlag[COPY]=dataSendTest(iocompParams,&streamParams->requestArray[COPY],); 
			// winTestInfo(ioParams, c); 
			// winTestInfo(ioParams, a); 
		}
		else
		{
			winActivateInfo(ioParams, b); 
		}
		dataSendInfo(ioParams);
		dataSendStart(ioParams, b); 
		scale(iocompParams, streamParams, iter, c, b);
		// scale(ioParams,iter, a,b,c); 
		dataSendEnd(ioParams, b); 	

		if(iter%WRITE_FREQ==0)
		{
			// copy_wait(iocompParams, streamParams, iter);
			scale_send(iocompParams, streamParams, iter, b );
		} 

//		/*
//		 * ADD(C) + MPITEST(B)
//		 * WAIT(B) 
//		 * SEND(C) 
//		 */ 
//		add(iocompParams, streamParams, iter, c, a, b);
//		if(iter%WRITE_FREQ==0)
//		{
//			scale_wait(iocompParams, streamParams, iter);
//			add_send(iocompParams, streamParams, iter, c );
//		} 
//
//		/*
//		 * TRIAD(A) + MPITEST(C)
//		 * WAIT(C)
//		 * SEND(A)
//		 */ 
//		triad(iocompParams, streamParams, iter, c, a, b);
//		if(iter%WRITE_FREQ==0)
//		{
//			add_wait(iocompParams, streamParams, iter);
//			triad_send(iocompParams, streamParams, iter, a);
//		} 
	} // end avg loop 

	triad_wait(iocompParams, streamParams, iter-1); // catch any triad sending after loop ends 

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



