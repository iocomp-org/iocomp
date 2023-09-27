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


	// a = iocompParams->array[0];
	// c = iocompParams->array[1];
	// b = iocompParams->array[2];
	b = iocompParams->array[0];
	
	// tempororary mallocing of c for testing with scale array 
	c = (double*)malloc(streamParams->localDataSize*sizeof(double)); // one rank only sends to one rank
	malloc_check(c); 
	a = (double*)malloc(streamParams->localDataSize*sizeof(double)); // one rank only sends to one rank
	malloc_check(a); 

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
			scale_wait(iocompParams, streamParams, iter, b); 
			// winTestInfo(iocompParams, c); 
			// winTestInfo(iocompParams, a); 
		}
		else
		{
			winActivateInfo(iocompParams, b); 
		}
		dataSendInfo(iocompParams);
#ifndef NDEBUG
		fprintf(iocompParams->debug, "stream-> after data send info \n");
#endif
		dataSendStart(iocompParams, b); 
#ifndef NDEBUG
		fprintf(iocompParams->debug, "stream-> after data send start\n");
#endif
		scale(iocompParams, streamParams, iter, c, b);

		scale_send(iocompParams, streamParams, iter, b );

		dataSendEnd(iocompParams, b); 	
#ifndef NDEBUG
		fprintf(iocompParams->debug, "stream->dataSendEnd function \n");
#endif


//		if(iter > 0)
//		{
//			// winWaitInfo(iocompParams, b); 
//			streamParams->mpiWaitFlag[SCALE]=dataSendTest(iocompParams,&streamParams->requestArray[SCALE],b); 
//			dataWait(iocompParams,&streamParams->requestArray[SCALE], b);
//			// streamParams->mpiWaitFlag[COPY]=dataSendTest(iocompParams,&streamParams->requestArray[COPY],); 
//			// winTestInfo(iocompParams, c); 
//			// winTestInfo(iocompParams, a); 
//		}
//		else
//		{
//			winActivateInfo(iocompParams, b); 
//		}
//		dataSendInfo(iocompParams);
//		printf("after data send info \n"); 
//		dataSendStart(iocompParams, b); 
//		printf("after data send start info \n"); 
//		scale(iocompParams, streamParams, iter, c, b);
//		printf("after scale \n"); 
//		// scale(iocompParams,iter, a,b,c); 
//		dataSendEnd(iocompParams, b); 	
//		printf("after datasendend  \n"); 
//
//		if(iter%WRITE_FREQ==0)
//		{
//			// copy_wait(iocompParams, streamParams, iter);
//			scale_send(iocompParams, streamParams, iter, b );
//		} 

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
#ifndef NDEBUG
		fprintf(iocompParams->debug, "stream-> after end of avg loop\n");
#endif

	// triad_wait(iocompParams, streamParams, iter-1); // catch any triad sending after loop ends 

//#ifndef NDEBUG
//	printf("After stopSend function\n"); 
//#endif
	/* send message to ioServer to free the windows and exit the recv loop */ 
	// winFreeInfo(iocompParams, a); 
	// winFreeInfo(iocompParams, c); 
	// winFreeInfo(iocompParams, b);
	
	scale_wait(iocompParams, streamParams, iter, b); 
	dataSendInfo(iocompParams); 
#ifndef NDEBUG
		// fprintf(iocompParams->debug, "stream->dataSendInfo for winFree assignment \n");
#endif

	stopSend(iocompParams); // send ghost message to stop MPI_Recvs and post win free for shared windows 
	// dataSendComplete(iocompParams, a); 
	// dataSendComplete(iocompParams, c); 
	// dataSendComplete(iocompParams, b); 
#ifndef NDEBUG
		fprintf(iocompParams->debug, "stream->data send complete\n");
#endif

	//wallTime_end = MPI_Wtime(); 
	//streamParams->wallTimer=wallTime_end - wallTime_start; 
	

	// abstracting function needed to free the arrays, as MPI winfree frees the
	// arrays  
	// free(a); 
	// free(b); 
	free(a); 
	a = NULL; 
	free(c); 
	c = NULL; 
	// a = NULL; 
	// b = NULL; 
//#ifndef NDEBUG
//		fprintf(iocompParams->debug, "stream->arrays freed\n");
//#endif

#ifndef NDEBUG
		fprintf(iocompParams->debug, "stream->compute step completed\n");
#endif
} 



