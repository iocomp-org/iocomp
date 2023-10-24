#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"

void add(struct iocomp_params *iocompParams, struct stream_params* streamParams, int iter, double* c, double* a, double* b)
{
#ifndef NDEBUG
	fprintf(streamParams->debug,"stream -> ADD starts\n"); 
#endif
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
//#endif 
		}
	} 
	streamParams->compTimer[ADD][iter] = MPI_Wtime() - timerStart;  // computeTime for ADD 
}

void add_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int iter, double* array, char* fileWrite)
{
#ifndef NDEBUG
	fprintf(streamParams->debug,"stream -> ADD wait\n"); 
#endif
	// wait for data from ADD(C) to be sent
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataWait(iocompParams,&streamParams->requestArray[ADD], array, fileWrite);
	streamParams->waitTimer[ADD][iter] = MPI_Wtime() - timerStart; // wait time for ADD
#ifndef NDEBUG
	fprintf(streamParams->debug,"stream -> ADD finished\n"); 
#endif
}

void add_send(struct iocomp_params *iocompParams, struct stream_params* streamParams, int iter, double* c)
{
#ifndef NDEBUG
	fprintf(streamParams->debug,"stream -> ADD send start\n"); 
#endif
	// wait for data from ADD(C) to be sent
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataSend(c,iocompParams, &streamParams->requestArray[ADD],streamParams->localDataSize); // send data off using dataSend
	streamParams->sendTimer[ADD][iter] = MPI_Wtime() - timerStart; // wait time for ADD
#ifndef NDEBUG
//	printf("stream -> ADD send finished with these values\n"); 
//	for(int i = 0; i< streamParams->localDataSize; i++){ printf("%lf",c[i]); }
//	printf("\n"); 
#endif
}
