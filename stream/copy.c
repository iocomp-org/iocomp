#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"

void copy(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* a)
{

#ifndef NDEBUG
	printf("stream -> COPY starts\n"); 
#endif
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	for(int i = 0; i< streamParams->localDataSize; i++)
	{
		c[i] = a[i]; 
#ifdef MPI_TESTS
		if(k>0) // need to wait for at least 1 iteration
		{
			streamParams->mpiWaitFlag[TRIAD] = dataSendTest(iocompParams,&streamParams->requestArray[TRIAD]);
			streamParams->mpiWaitFlag[SCALE] = dataSendTest(iocompParams,&streamParams->requestArray[SCALE]);
		} 
#endif 
	}
	streamParams->compTimer[COPY][k] = MPI_Wtime() - timerStart;  // computeTime for COPY 

	if(k%streamParams->writeFreq == 0)
	{
		timerStart = MPI_Wtime(); // timer start for dataSend 
		dataSend(c,iocompParams, &streamParams->requestArray[COPY],streamParams->localDataSize); // send data off using dataSend
		int counter = (int)k/streamParams->writeFreq; // counter for timers  
		streamParams->sendTimer[COPY][counter] = MPI_Wtime() - timerStart; // send time for COPY 
#ifndef NDEBUG
		printf("stream -> COPY finished with elements: \n"); 
		for(int i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",c[i]); }
#endif
	} 
}

void copy_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k)
{
#ifndef NDEBUG
	printf("stream -> COPY wait starts \n"); 
#endif
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataWait(iocompParams,&streamParams->requestArray[COPY]);
	streamParams->waitTimer[COPY][k] = MPI_Wtime() - timerStart; // wait time for COPY
#ifndef NDEBUG
	printf("stream -> COPY wait end \n"); 
#endif
}
