#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"
#define constant 5.00

void scale(struct iocomp_params *iocompParams, struct stream_params* streamParams, int iter, double* c, double* b)
{
	if(streamParams->verboseFlag){
		fprintf(streamParams->debug, "stream -> SCALE starts\n");
	}
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	for(int loop = 0; loop < COMPLOOPCOUNT; loop++) 
	{
		for(int i = 0; i< streamParams->localDataSize; i++) // scale kernel
		{
			b[i] = constant * c[i]; 
			//#ifdef MPI_TESTS
			//			if(iter%WRITE_FREQ == 0) // only when scale sends data 
			//			{
			//				streamParams->mpiWaitFlag[COPY]=dataSendTest(iocompParams,&streamParams->requestArray[COPY]); 
			//			} 
			//} 
	}
} 
streamParams->compTimer[SCALE][iter] = MPI_Wtime() - timerStart;  // computeTime for SCALE 
}

void scale_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int iter, double* b, char* fileWrite)
{
	if(streamParams->verboseFlag){
		fprintf(streamParams->debug,"stream -> SCALE wait\n"); 
	}
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataWait(iocompParams,&streamParams->requestArray[SCALE], b, fileWrite);
	streamParams->waitTimer[SCALE][iter] = MPI_Wtime() - timerStart; // wait time for SCALE
	if(streamParams->verboseFlag){
		fprintf(streamParams->debug,"stream -> SCALE wait finished\n"); 
	}
}

void scale_send(struct iocomp_params *iocompParams, struct stream_params* streamParams, int iter, double* b)
{
	if(streamParams->verboseFlag){
		fprintf(streamParams->debug,"stream -> SCALE send start\n"); 
	}
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataSend(b,iocompParams, &streamParams->requestArray[SCALE],streamParams->localDataSize); // send data off using dataSend
	streamParams->sendTimer[SCALE][iter] = MPI_Wtime() - timerStart; // wait time for ADD
}
