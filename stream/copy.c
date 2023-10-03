#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"

void copy(struct iocomp_params *iocompParams, struct stream_params* streamParams, int iter, double* c, double* a)
{
#ifndef NDEBUG
	fprintf(iocompParams->debug,"stream -> COPY starts\n"); 
#endif
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	for(int loop = 0; loop < COMPLOOPCOUNT; loop++) 
	{
		for(int i = 0; i< streamParams->localDataSize; i++)
		{
			c[i] = a[i]; 
//#ifdef MPI_TESTS
//			if( (iter+1)%WRITE_FREQ == 0 && iter > 0) // test for triad which sent message in previous loop
//			{
//				streamParams->mpiWaitFlag[TRIAD] = dataSendTest(iocompParams,&streamParams->requestArray[TRIAD]);
//			} 
//#endif 
		}
	} 
	streamParams->compTimer[COPY][iter] = MPI_Wtime() - timerStart;  // computeTime for COPY 
}

void copy_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k)
{
#ifndef NDEBUG
	fprintf(iocompParams->debug,"stream -> COPY wait starts \n"); 
#endif
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataWait(iocompParams,&streamParams->requestArray[COPY]);
	streamParams->waitTimer[COPY][k] = MPI_Wtime() - timerStart; // wait time for COPY
#ifndef NDEBUG
	fprintf(iocompParams->debug,"stream -> COPY wait end \n"); 
#endif
}


void copy_send(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c)
{
#ifndef NDEBUG
	fprintf(iocompParams->debug,"stream -> COPY send start\n"); 
#endif
	// wait for data from ADD(C) to be sent
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataSend(c,iocompParams, &streamParams->requestArray[COPY],streamParams->localDataSize); // send data off using dataSend
	streamParams->sendTimer[COPY][k] = MPI_Wtime() - timerStart; // wait time for ADD
#ifndef NDEBUG
//	printf("stream -> COPY send finished with values\n"); 
//	for(int i = 0; i< streamParams->localDataSize; i++){ printf("%lf",c[i]); }
//	printf("\n"); 
#endif
}
