#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"
#define constant 3.0 

void triad(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* a, double* b )
{
#ifndef NDEBUG
	printf("stream -> TRIAD starts\n"); 
#endif
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	for(int loop = 0; loop < COMPLOOPCOUNT; loop++) 
	{
		for(int i = 0; i< streamParams->localDataSize; i++) // triad kernel 
		{
			a[i] = b[i] + c[i] * constant;  
#ifdef MPI_TESTS
			streamParams->mpiWaitFlag[ADD]=dataSendTest(iocompParams,&streamParams->requestArray[ADD]);  
#endif 
		}
	} 
	streamParams->compTimer[TRIAD][k] = MPI_Wtime() - timerStart;  // computeTime for TRIAD 
}

void triad_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k)
{
#ifndef NDEBUG
	printf("stream -> TRIAD wait starts \n"); 
#endif
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataWait(iocompParams,&streamParams->requestArray[TRIAD]);
	streamParams->waitTimer[TRIAD][k] = MPI_Wtime() - timerStart; // wait time for TRIAD
#ifndef NDEBUG
	printf("stream -> TRIAD wait end \n"); 
#endif
}

void triad_send(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double*a)
{
#ifndef NDEBUG
	printf("stream -> TRIAD send start\n"); 
#endif
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataSend(a,iocompParams, &streamParams->requestArray[TRIAD],streamParams->localDataSize); // send data off using dataSend
	streamParams->sendTimer[TRIAD][k] = MPI_Wtime() - timerStart; // wait time for ADD
#ifndef NDEBUG
	printf("stream -> TRIAD send finished\n"); 
	for(int i = 0; i< streamParams->localDataSize; i++){ printf("%lf",a[i]); }
	printf("\n"); 
#endif
}
