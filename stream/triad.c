#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"
#define constant 3.0 

void triad(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* a, double* b )
{

	// timerStart = timer_start(computeRank); // start timing 
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	for(int i = 0; i< streamParams->localDataSize; i++)
	{
		a[i] = b[i] + c[i] * constant;  
#ifdef MPI_TESTS
		streamParams->mpiWaitFlag[ADD]=dataSendTest(iocompParams,&streamParams->requestArray[ADD]);  
		streamParams->mpiWaitFlag[SCALE]=dataSendTest(iocompParams,&streamParams->requestArray[SCALE]);  
#endif 
	}
	streamParams->compTimer[TRIAD][k] = MPI_Wtime() - timerStart;  // computeTime for TRIAD 

	if(k%streamParams->writeFreq == 0)
	{
		timerStart = MPI_Wtime(); // timer start for dataSend 
		dataSend(a,iocompParams, &streamParams->requestArray[TRIAD],streamParams->localDataSize); // send data off using dataSend
		int counter = (int)k/streamParams->writeFreq; // counter for timers  
		streamParams->sendTimer[TRIAD][counter] = MPI_Wtime() - timerStart; // send time for TRIAD 
#ifndef NDEBUG
		printf("STREAM -> TRIAD finished with elements:\n"); 
		for(int i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",a[i]); }
#endif
	} 
}

void triad_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k)
{
	// wait for data from TRIAD(C) to be sent
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataWait(iocompParams,&streamParams->requestArray[TRIAD]);
	streamParams->waitTimer[TRIAD][k] = MPI_Wtime() - timerStart; // wait time for TRIAD
}
