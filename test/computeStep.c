/* Inspired from the STREAM benchmark */ 
/* github url https://github.com/jeffhammond/STREAM.git */ 

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "test.h"

#define COPY		0
#define SCALE		1
#define ADD			2
#define TRIAD		3

double timer_start(int computeRank)
{
	double timerStart = 0.0; 	
	if (computeRank == 0) // timing will be measured by using ioRank = 0 
	{	
		timerStart = MPI_Wtime();
	}
	return(timerStart); 
}

double timer_end(double timerStart, int computeRank )
{	
	double timeDiff = 0.0; 
	if (computeRank == 0) // timing will be measured by using ioRank = 0 
	{	
		timeDiff = MPI_Wtime() - timerStart; 
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
	double constant = 3.0; 
	double c[iocompParams->localDataSize]; 
	double b[iocompParams->localDataSize]; 
	int computeRank;
	MPI_Comm comm; 
	comm = iocompParams->globalComm; 
	ierr = MPI_Comm_rank(comm, &computeRank); 
	mpi_error_check(ierr); 
	double timerStart, timerEnd, timer[KERNELS][iter], totalTimer[KERNELS][iter], programStartTime, programEndTime, waitTimer[KERNELS][iter], wallTime; 
	timerStart = 0.0; 
	timerEnd = 0.0; 
	MPI_Request requestArray[KERNELS]; 
	int mpiWaitFlag[KERNELS]; 

	// initialise all timers to be 0 
	for (i=0; i < KERNELS; i++) 
	{
		for (k =0; k<iter; k++)
		{
			timer[i][k] = 0.0;
			totalTimer[i][k] = 0.0;
			waitTimer[i][k] = 0.0;
		}
	} 

#ifndef NDEBUG
	printf("After inits\n"); 
#endif
	programStartTime = MPI_Wtime(); 	
	for(k = 0; k< iter; k++) // averaging 
	{
		// wait for data from ADD(C) to be sent 
		if(k>0)
		{
			if(mpiWaitFlag[ADD] == 0 && iocompParams->hyperthreadFlag!= 0) {printf("ADD data not sent \n");}  // test if HT flag is on
			timerStart = timer_start(computeRank); // start timing 
			dataWait(iocompParams,&requestArray[ADD]);
			waitTimer[ADD][k] = timer_end(timerStart,computeRank); // wait time for ADD
		} 

		/*
		* COPY
		*/ 
		timerStart = timer_start(computeRank); // start timing 
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			c[i] = a[i]; 
			if(k>0){mpiWaitFlag[TRIAD] = dataSendTest(iocompParams,&requestArray[TRIAD]);} // test if TRIAD data got sent
		}
		timer[COPY][k] = timer_end(timerStart,computeRank); // computeTime for COPY 
		dataSend(c,iocompParams, &requestArray[COPY]); // send data off using dataSend
		totalTimer[COPY][k] = timer_end(timerStart,computeRank); // total time for COPY  
#ifndef NDEBUG
		for(i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",c[i]); }
		printf("After COPY\n"); 
#endif

		// wait for data from previous SCALE(B) to be sent 
		if(k>0)
		{
			if(mpiWaitFlag[SCALE] == 0 && iocompParams->hyperthreadFlag!= 0) {printf("SCALE data not sent \n");}  // test if HT flag is on
			timerStart = timer_start(computeRank); // start timing 
			dataWait(iocompParams,&requestArray[SCALE]);
			waitTimer[SCALE][k] = timer_end(timerStart,computeRank); // wait time for SCALE 
		} 

		/*
		* SCALE
		*/ 
		timerStart = timer_start(computeRank); // start timing 
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			b[i] = constant * c[i]; 
			mpiWaitFlag[COPY]=dataSendTest(iocompParams,&requestArray[0]); // test if COPY data got sent 
		}
		timer[SCALE][k] = timer_end(timerStart,computeRank); // computeTime for SCALE
		dataSend(b,iocompParams, &requestArray[SCALE]); // send data off using dataSend
		totalTimer[SCALE][k] = timer_end(timerStart,computeRank); // total time for SCALE
#ifndef NDEBUG
		for(i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",b[i]); }
		printf("After SCALE\n"); 
#endif
		
		// wait for data from COPY(C) to be sent
		if(mpiWaitFlag[COPY] == 0 && iocompParams->hyperthreadFlag!= 0) {printf("COPY data not sent \n");}  // test if HT flag is on
		timerStart = timer_start(computeRank); // start timing 
		dataWait(iocompParams,&requestArray[COPY]);
		waitTimer[COPY][k] = timer_end(timerStart,computeRank); // wait time for COPY

		/*
		* ADD
		*/ 
		timerStart = timer_start(computeRank); // start timing 
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			c[i] = a[i] + b[i]; 
			mpiWaitFlag[SCALE]=dataSendTest(iocompParams,&requestArray[SCALE]); // test if SCALE data got sent  
		}
		timer[ADD][k] = timer_end(timerStart,computeRank); // computeTime for ADD
		dataSend(c,iocompParams, &requestArray[ADD]); // send data off using dataSend
		totalTimer[ADD][k] = timer_end(timerStart,computeRank); // total time for ADD
#ifndef NDEBUG
		for(i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",c[i]); }
		printf("After ADD\n"); 
#endif
		
		// wait for data from previous TRIAD(A) to be sent 
		if(k>0)
		{
			if(mpiWaitFlag[TRIAD] == 0 && iocompParams->hyperthreadFlag!= 0) {printf("TRIAD data not sent \n");}  // test if HT flag is on
			timerStart = timer_start(computeRank); // start timing 
			dataWait(iocompParams,&requestArray[TRIAD]);
			waitTimer[TRIAD][k] = timer_end(timerStart,computeRank); // wait time for SCALE 
		} 

		/*
		* TRIAD 
		*/ 
		timerStart = timer_start(computeRank); // start timing 
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			a[i] = b[i] + c[i] * constant;  
			mpiWaitFlag[ADD]=dataSendTest(iocompParams,&requestArray[ADD]); // test if ADD data got sent  
		}
		timer[TRIAD][k] = timer_end(timerStart,computeRank); // computeTime for TRIAD
		dataSend(a,iocompParams, &requestArray[TRIAD]); // send data
		totalTimer[TRIAD][k] = timer_end(timerStart,computeRank); // total time for TRIAD
#ifndef NDEBUG
		for(i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",a[i]); }
		printf("After TRIAD\n"); 
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



