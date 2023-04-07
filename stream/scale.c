#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"
#define constant 5.00
void scale(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* b)
{

	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	for(int i = 0; i< streamParams->localDataSize; i++)
	{
		b[i] = constant * c[i]; 
#ifdef MPI_TESTS
		streamParams->mpiWaitFlag[COPY]=dataSendTest(iocompParams,&streamParams->requestArray[COPY]); 
		if(k > 0) // need to wait for at least 1 occurance of triad 
		{ 	
			streamParams->mpiWaitFlag[TRIAD]=dataSendTest(iocompParams,&streamParams->requestArray[TRIAD]); 
		} 
#endif 
	}
	streamParams->compTimer[SCALE][k] = MPI_Wtime() - timerStart;  // computeTime for SCALE 

	if(k%streamParams->writeFreq == 0)
	{
		timerStart = MPI_Wtime(); // timer start for dataSend 
		dataSend(b,iocompParams, &streamParams->requestArray[SCALE],streamParams->localDataSize); // send data off using dataSend
		int counter = (int)k/streamParams->writeFreq; // counter for timers  
		streamParams->sendTimer[SCALE][counter] = MPI_Wtime() - timerStart; // send time for SCALE 
#ifndef NDEBUG
		printf("STREAM -> SCALE finished with elements: \n"); 
		for(int i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",c[i]); }
#endif
	} 
}

void scale_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k)
{
	// wait for data from SCALE(C) to be sent
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataWait(iocompParams,&streamParams->requestArray[SCALE]);
	streamParams->waitTimer[SCALE][k] = MPI_Wtime() - timerStart; // wait time for SCALE
}
