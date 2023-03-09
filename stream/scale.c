#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"
#define constant 5.00
void scale(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* b)
{

	// timerStart = timer_start(computeRank); // start timing 
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	for(int i = 0; i< streamParams->localDataSize; i++)
	{
			b[i] = constant * c[i]; 
//			mpiWaitFlag[COPY]=dataSendTest(iocompParams,&requestArray[0]); // test if COPY data got sent 
	}
	streamParams->compTimer[SCALE][k] = MPI_Wtime() - timerStart;  // computeTime for SCALE 
//	dataSend(c,iocompParams, &streamParams->requestArray[SCALE],streamParams->localDataSize); // send data off using dataSend
	streamParams->sendTimer[SCALE][k] = MPI_Wtime() - timerStart; // send time for SCALE 
#ifndef NDEBUG
	for(int i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",c[i]); }
	printf("After SCALE\n"); 
#endif
}

void scale_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k)
{
	// wait for data from SCALE(C) to be sent
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataWait(iocompParams,&streamParams->requestArray[SCALE]);
	streamParams->waitTimer[SCALE][k] = MPI_Wtime() - timerStart; // wait time for SCALE
}
