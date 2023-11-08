/**	@file computeStep.c
 *	@brief Compute step calling the different STREAM kernels 
 * 	Inspired from the STREAM benchmark 
 * 	github url https://github.com/jeffhammond/STREAM.git 
 *	@author Shrey (sb15895) 
 */ 

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <inttypes.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"

void computeStep(struct iocomp_params *iocompParams, struct stream_params *streamParams, MPI_Comm comm)
{
	if(streamParams->verboseFlag){
		fprintf(streamParams->debug, "stream-> starting compute step  \n");
	}

	/*
	 * mallocing the pointers 
	 */ 
	double* b = NULL; // initialise data pointer  
	double* c = NULL; // initialise data pointer  
	double* a = NULL; // initialise data pointer  

	// extension for filenames 
	char ext[5][5] = {".dat", ".h5", ".h5", "", ""};

	/*
	 * Initialise a, b, c. 
	 * Initialise shared windows associated with arrays *if* shared flag is true
	 * otherwise return the malloced arrays with localDataSize 
	 */ 
	winInits(iocompParams, (int)streamParams->localDataSize); 
	
	b = iocompParams->array[0];
	c = iocompParams->array[1];
	a = iocompParams->array[2];

	if(streamParams->verboseFlag){
		fprintf(streamParams->debug, "stream->arrays malloced \n");
		for(int i = 0; i < NUMWIN; i++)
		{
			fprintf(streamParams->debug, "Memory address of array %i 0x%" PRIXPTR "\n",i, (uintptr_t)iocompParams->array[i]);
		} 
	}

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
		if(streamParams->verboseFlag){
			fprintf(streamParams->debug, "stream-> stream loop starts \n");
		}

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
			scale_wait(iocompParams, streamParams, iter, b, fileWrite_SCALE); 
			winActivateInfo(iocompParams, b); 
			winTestInfo(iocompParams, c); 
			winTestInfo(iocompParams, a); 
		}
		else
		{
			winActivateInfo(iocompParams, b); 
		}
		snprintf(fileWrite_SCALE, sizeof(fileWrite_SCALE), "B_%i%s",iter, ext[streamParams->io]);
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
		snprintf(fileWrite_ADD, sizeof(fileWrite_ADD), "C_%i%s",iter, ext[streamParams->io]);
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
		snprintf(fileWrite_TRIAD, sizeof(fileWrite_TRIAD), "A_%i%s",iter,ext[streamParams->io]);
		preDataSend(iocompParams, a, fileWrite_TRIAD); 
		triad(iocompParams, streamParams, iter, c, a, b);
		triad_send(iocompParams, streamParams, iter, a);

	} // end avg loop 
	if(streamParams->verboseFlag){
		fprintf(streamParams->debug, "stream-> after end of avg loop\n");
	}

	if(streamParams->HT_flag)
	{
		scale_wait(iocompParams, streamParams, iter, b, fileWrite_SCALE); 
		add_wait(iocompParams, streamParams, iter, c, fileWrite_ADD); 
		triad_wait(iocompParams, streamParams, iter, a, fileWrite_TRIAD); 
	} 
	stopSend(iocompParams); // send ghost message to stop MPI_Recvs and post win free for shared windows 
	if(streamParams->verboseFlag){
		fprintf(streamParams->debug, "stream->data send complete\n");
	}

	winFinalise(iocompParams); // finalise arrays 
	wallTime_end = MPI_Wtime(); 
	assert(wallTime_end!=wallTime_start); 
	streamParams->wallTimer=wallTime_end - wallTime_start; 

	if(streamParams->verboseFlag){
		fprintf(streamParams->debug, "stream->compute step completed\n");
	}
} 



