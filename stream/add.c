#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"

void add(struct iocomp_params *iocompParams, struct stream_params* streamParams, int iter, double* c, double* a, double* b)
{
	if(streamParams->verboseFlag){
		fprintf(streamParams->debug,"stream -> ADD starts\n"); 
	}
	double timerStart = 0.0;
	timerStart = MPI_Wtime(); 
	for(int loop = 0; loop < COMPLOOPCOUNT; loop++) 
	{
		for(int i = 0; i< streamParams->localDataSize; i++) // actual copy kernel 
		{
			c[i] = a[i] + b[i]; 
			//#ifdef MPI_TESTS
			//			if(iter%WRITE_FREQ == 0) // only when scale sends data 
			//			{
			//				streamParams->mpiWaitFlag[SCALE]=dataSendTest(iocompParams,&streamParams->requestArray[SCALE]); 
			//			} 
			//} 
	}
} 
streamParams->compTimer[ADD][iter] = MPI_Wtime() - timerStart;  // computeTime for ADD 
}

void add_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int iter, double* array, char* fileWrite)
{
	if(streamParams->verboseFlag){
		fprintf(streamParams->debug,"stream -> ADD wait\n"); 
	}
	// wait for data from ADD(C) to be sent
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataWait(iocompParams,&streamParams->requestArray[ADD], array, fileWrite);
	streamParams->waitTimer[ADD][iter] = MPI_Wtime() - timerStart; // wait time for ADD
	if(streamParams->verboseFlag){
		fprintf(streamParams->debug,"stream -> ADD finished\n"); 
	}
}

void add_send(struct iocomp_params *iocompParams, struct stream_params* streamParams, int iter, double* c)
{
	if(streamParams->verboseFlag){
		fprintf(streamParams->debug,"stream -> ADD send start\n"); 
	}
	// wait for data from ADD(C) to be sent
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataSend(c,iocompParams, &streamParams->requestArray[ADD],streamParams->localDataSize); // send data off using dataSend
	streamParams->sendTimer[ADD][iter] = MPI_Wtime() - timerStart; // wait time for ADD
}
