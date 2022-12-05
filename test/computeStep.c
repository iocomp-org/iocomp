/* Inspired from the STREAM benchmark */ 
/* github url https://github.com/jeffhammond/STREAM.git */ 

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "test.h"

void timer_start(double* timerStart, int computeRank)
{
	if (computeRank == 0) // timing will be measured by using ioRank = 0 
	{	
		*timerStart = MPI_Wtime();
	}
}

double timer_end(double* timerStart, int computeRank )
{	
	double timeDiff = 0.0; 
	if (computeRank == 0) // timing will be measured by using ioRank = 0 
	{	
		timeDiff = MPI_Wtime() - *timerStart; 
	} 
	return(timeDiff); 
}

void stream(double* a, struct iocomp_params *iocompParams)
{
#ifndef NDEBUG
	printf("stream starts\n"); 
#endif
	if (a == NULL)
	{
		printf("data is null \n"); 
		return; 
	}
	int i, ierr,k; 
	int constant = 5; 
	double c[iocompParams->localDataSize]; 
	double b[iocompParams->localDataSize]; 
	int computeRank;
	MPI_Comm comm; 
	comm = iocompParams->globalComm; 
	printf("comm\n"); 
	ierr = MPI_Comm_rank(comm, &computeRank); 
	mpi_error_check(ierr); 
	double timerStart, timerEnd, timer[4][iter], totalTimer[4][iter], programStartTime, programEndTime, waitTimer[4][iter], wallTime; 
	timerStart = 0.0; 
	timerEnd = 0.0; 
	printf("Before request array \n"); 
	MPI_Request requestArray[4]; 
	int mpiWaitFlag; 

#ifndef NDEBUG
	printf("After inits\n"); 
#endif
	programStartTime = MPI_Wtime(); 	
	for(k = 0; k< iter; k++) // averaging 
	{
		// wait for data from triad(C) to be sent 
		if(k>0)
		{
			timer_start(&timerStart,computeRank); // start timing 
			dataWait(iocompParams,&requestArray[3]);
			waitTimer[3][k] = timer_end(&timerStart,computeRank); // wait time for scale 
		} 

		/*
		* COPY
		*/ 
		timer_start(&timerStart,computeRank); // start timing 
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			c[i] = a[i]; 
		}
		timer[0][k] = timer_end(&timerStart,computeRank); // computational time for copy 
		// end of computation
		dataSend(c,iocompParams, &requestArray[0]); // send data off using dataSend
		totalTimer[0][k] = timer_end(&timerStart,computeRank); // total time for copy  
#ifndef NDEBUG
		for(i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",c[i]); }
		printf("After copy\n"); 
#endif

		// wait for data from previous scale(B) to be sent 
		if(k>0)
		{
			timer_start(&timerStart,computeRank); // start timing 
			dataWait(iocompParams,&requestArray[1]);
			waitTimer[1][k] = timer_end(&timerStart,computeRank); // wait time for scale 
		} 

		/*
		* SCALE
		*/ 
		timer_start(&timerStart,computeRank); // start timing 
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			b[i] = constant * c[i]; 
			dataSendTest(iocompParams,&requestArray[0]); // test if copy data got sent 
		}
		timer[1][k] = timer_end(&timerStart,computeRank); // computational time for scale
		dataSend(b,iocompParams, &requestArray[1]); // send data off using dataSend
		totalTimer[1][k] = timer_end(&timerStart,computeRank); // total time for scale
#ifndef NDEBUG
		for(i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",b[i]); }
		printf("After scale\n"); 
#endif
		
		// wait for copy array (c) to be sent 
		timer_start(&timerStart,computeRank); // start timing 
		dataWait(iocompParams,&requestArray[0]);
		waitTimer[0][k] = timer_end(&timerStart,computeRank); // wait time for copy 

		/*
		* ADD
		*/ 
		timer_start(&timerStart,computeRank); // start timing 
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			c[i] = a[i] + b[i]; 
			dataSendTest(iocompParams,&requestArray[1]); // test if scale data got sent  
		}
		timer[2][k] = timer_end(&timerStart,computeRank); // computeTime for add
		dataSend(c,iocompParams, &requestArray[2]); // send data off using dataSend
		totalTimer[2][k] = timer_end(&timerStart,computeRank); // total time for add
#ifndef NDEBUG
		for(i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",c[i]); }
		printf("After add\n"); 
#endif
		
		// wait for data from previous triad(A) to be sent 
		if(k>0)
		{
			timer_start(&timerStart,computeRank); // start timing 
			dataWait(iocompParams,&requestArray[3]);
			waitTimer[3][k] = timer_end(&timerStart,computeRank); // wait time for scale 
		} 

		/*
		* TRIAD 
		*/ 
		timer_start(&timerStart,computeRank); // start timing 
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			a[i] = b[i] + c[i] * constant;  
			dataSendTest(iocompParams,&requestArray[2]); // test if previous data got sent  
		}
		timer[3][k] = timer_end(&timerStart,computeRank); // computeTime for triad
		dataSend(a,iocompParams, &requestArray[3]); // send data
		totalTimer[3][k] = timer_end(&timerStart,computeRank); // total time for triad
#ifndef NDEBUG
		for(i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",a[i]); }
		printf("After triad\n"); 
#endif
	} // end avg loop  

	stopSend(iocompParams); // send ghost message to stop MPI_Recvs 
#ifndef NDEBUG
	printf("After stopSend function\n"); 
#endif

	if(computeRank == 0)
	{
		programEndTime = MPI_Wtime();
		wallTime = programEndTime - programStartTime; 
		resultsOutput(timer, totalTimer, waitTimer,wallTime); // write to csv file for compute write 
	} 

}

void computeStep(double* data, struct iocomp_params *iocompParams)
{
	int i, globalMPIRank, ierr;   

	ierr = MPI_Comm_rank(iocompParams->globalComm, &globalMPIRank); 
	mpi_error_check(ierr); 
	for(i = 0; i < iocompParams->localDataSize; i++)
	{
		data[i] =iocompParams->localDataSize * globalMPIRank + i; 
	}
	stream(data,iocompParams); 
#ifndef NDEBUG
	printf("After stream\n"); 
#endif
} 



