#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"

void add(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* a, double* b)
{
#ifndef NDEBUG
	printf("stream -> ADD starts\n"); 
#endif

	// timerStart = timer_start(computeRank); // start timing 
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	for(int i = 0; i< streamParams->localDataSize; i++)
	{
		c[i] = a[i] + b[i]; 
		streamParams->mpiWaitFlag[SCALE]=dataSendTest(iocompParams,&streamParams->requestArray[SCALE]); // test if SCALE data got sent  
	}
	streamParams->compTimer[ADD][k] = MPI_Wtime() - timerStart;  // computeTime for ADD 
	dataSend(c,iocompParams, &streamParams->requestArray[ADD],streamParams->localDataSize); // send data off using dataSend
	streamParams->sendTimer[ADD][k] = MPI_Wtime() - timerStart; // send time for ADD 
#ifndef NDEBUG
	printf("ADD finished with elements: \n"); 
	for(int i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",c[i]); }
#endif
}

void add_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k)
{
#ifndef NDEBUG
	printf("stream -> ADD wait\n"); 
#endif
	// wait for data from ADD(C) to be sent
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataWait(iocompParams,&streamParams->requestArray[ADD]);
	streamParams->waitTimer[ADD][k] = MPI_Wtime() - timerStart; // wait time for ADD
#ifndef NDEBUG
	printf("stream -> ADD finished\n"); 
#endif
}
