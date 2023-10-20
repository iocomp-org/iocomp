#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"

void triad(struct iocomp_params *iocompParams, struct stream_params* streamParams, int iter, double* c, double* a, double* b )
{
#ifndef NDEBUG
	fprintf(iocompParams->debug,"stream -> TRIAD starts\n"); 
#endif
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	for(int loop = 0; loop < COMPLOOPCOUNT; loop++) 
	{
		for(int i = 0; i< streamParams->localDataSize; i++) // triad kernel 
		{
			a[i] = b[i] + c[i] * CONSTANT;  
//#ifdef MPI_TESTS
//			if(iter%WRITE_FREQ == 0) // only when scale sends data 
//			{
//				streamParams->mpiWaitFlag[ADD]=dataSendTest(iocompParams,&streamParams->requestArray[ADD]);  
//			} 
//#endif 
		}
	} 
	streamParams->compTimer[TRIAD][iter] = MPI_Wtime() - timerStart;  // computeTime for TRIAD 
}

void triad_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int iter, double* array)
{
#ifndef NDEBUG
	fprintf(iocompParams->debug,"stream -> TRIAD wait starts \n"); 
#endif
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataWait(iocompParams,&streamParams->requestArray[TRIAD], array);
	streamParams->waitTimer[TRIAD][iter] = MPI_Wtime() - timerStart; // wait time for TRIAD
#ifndef NDEBUG
	fprintf(iocompParams->debug,"stream -> TRIAD wait end \n"); 
#endif
}

void triad_send(struct iocomp_params *iocompParams, struct stream_params* streamParams, int iter, double*a)
{
#ifndef NDEBUG
	fprintf(iocompParams->debug,"stream -> TRIAD send start\n"); 
#endif
	double timerStart = 0.0; 
	timerStart = MPI_Wtime(); 
	dataSend(a,iocompParams, &streamParams->requestArray[TRIAD],streamParams->localDataSize); // send data off using dataSend
	streamParams->sendTimer[TRIAD][iter] = MPI_Wtime() - timerStart; // wait time for ADD
#ifndef NDEBUG
//	fprintf(iocompParams->debug,"stream -> TRIAD send finished\n"); 
//	for(int i = 0; i< streamParams->localDataSize; i++){ printf("%lf",a[i]); }
//	printf("\n"); 
#endif
}
