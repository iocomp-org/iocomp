#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"

void stream(double* a, struct iocomp_params *iocompParams, struct stream_params *streamParams, MPI_Comm comm)
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
	double c[streamParams->localDataSize]; 
	double b[streamParams->localDataSize]; 
	int computeRank;

	ierr = MPI_Comm_rank(comm, &computeRank); 
	mpi_error_check(ierr); 

	// initialise all timers to be 0 
	for (i=0; i < KERNELS; i++) 
	{
		for (k =0; k<iter; k++)
		{
			streamParams->compTimer[i][k] = 0.0;
			streamParams->sendTimer[i][k] = 0.0;
			streamParams->waitTimer[i][k] = 0.0;
		}
	} 

#ifndef NDEBUG
	printf("After inits\n"); 
#endif
	
	double programStartTime, programEndTime, wallTime; 
	programStartTime = MPI_Wtime(); 	
	for(k = 0; k< LOOPCOUNT; k++) // averaging 
	{
#ifndef NDEBUG
	printf("stream loop starts\n"); 
#endif
		// wait for data from ADD(C) to be sent 
		/*
		if(k>0)
		{
			if(mpiWaitFlag[ADD] == 0 && iocompParams->hyperthreadFlag!= 0) {printf("ADD data not sent \n");}  // test if HT flag is on
			timerStart = timer_start(computeRank); // start timing 
			dataWait(iocompParams,&requestArray[ADD]);
			streamParams->waitTimer[ADD][k] = timer_end(timerStart,computeRank); // wait time for ADD
		} 
		*/ 

		/*
		* COPY
		*/ 
		copy(iocompParams, streamParams, k, c, a); // send copy data and get timers for send and compute  
		copy_wait(iocompParams, streamParams, k); // wait for copy to be send its data 
		
		/*
		* SCALE
		*/ 
		scale(iocompParams, streamParams, k, c, b); // send scale data and get timers for send and compute  
		scale_wait(iocompParams, streamParams, k); // wait for send to be finished sending its data 

		/*
		* ADD
		*/ 
		add(iocompParams, streamParams, k, c, a, b); // send copy data and get timers for send and compute  
		add_wait(iocompParams, streamParams, k); // wait for copy to be send its data 

		/*
		* TRIAD 
		*/ 
		triad(iocompParams, streamParams, k, c, a, b); // send copy data and get timers for send and compute  
		triad_wait(iocompParams, streamParams, k); // wait for copy to be send its data 

//		// wait for data from previous SCALE(B) to be sent 
//		if(k>0)
//		{
//			if(mpiWaitFlag[SCALE] == 0 && iocompParams->hyperthreadFlag!= 0) {printf("SCALE data not sent \n");}  // test if HT flag is on
//			timerStart = timer_start(computeRank); // start timing 
//			dataWait(iocompParams,&requestArray[SCALE]);
//			streamParams->waitTimer[SCALE][k] = timer_end(timerStart,computeRank); // wait time for SCALE 
//		} 
//
//		/*
//		* SCALE
//		*/ 
//		timerStart = timer_start(computeRank); // start timing 
//		for(i = 0; i< iocompParams->localDataSize; i++)
//		{
//			b[i] = constant * c[i]; 
//			mpiWaitFlag[COPY]=dataSendTest(iocompParams,&requestArray[0]); // test if COPY data got sent 
//		}
//		streamParams->compTimer[SCALE][k] = timer_end(timerStart,computeRank); // computeTime for SCALE
//		dataSend(b,iocompParams, &requestArray[SCALE],  streamParams->localDataSize); // send data off using dataSend
//		streamParams->sendTimer[SCALE][k] = timer_end(timerStart,computeRank); // send time for SCALE
//#ifndef NDEBUG
//		for(i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",b[i]); }
//		printf("After SCALE\n"); 
//#endif
//		
//		// wait for data from COPY(C) to be sent
//		if(mpiWaitFlag[COPY] == 0 && iocompParams->hyperthreadFlag!= 0) {printf("COPY data not sent \n");}  // test if HT flag is on
//		timerStart = timer_start(computeRank); // start timing 
//		dataWait(iocompParams,&requestArray[COPY]);
//		streamParams->waitTimer[COPY][k] = timer_end(timerStart,computeRank); // wait time for COPY
//
//		/*
//		* ADD
//		*/ 
			
//		timerStart = timer_start(computeRank); // start timing 
//		for(i = 0; i< iocompParams->localDataSize; i++)
//		{
//			c[i] = a[i] + b[i]; 
//			mpiWaitFlag[SCALE]=dataSendTest(iocompParams,&requestArray[SCALE]); // test if SCALE data got sent  
//		}
//		streamParams->compTimer[ADD][k] = timer_end(timerStart,computeRank); // computeTime for ADD
//		dataSend(c,iocompParams, &requestArray[ADD], streamParams->localDataSize); // send data off using dataSend
//		streamParams->sendTimer[ADD][k] = timer_end(timerStart,computeRank); // send time for ADD
//#ifndef NDEBUG
//		for(i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",c[i]); }
//		printf("After ADD\n"); 
//#endif
//		
//		// wait for data from previous TRIAD(A) to be sent 
//		if(k>0)
//		{
//			if(mpiWaitFlag[TRIAD] == 0 && iocompParams->hyperthreadFlag!= 0) {printf("TRIAD data not sent \n");}  // test if HT flag is on
//			timerStart = timer_start(computeRank); // start timing 
//			dataWait(iocompParams,&requestArray[TRIAD]);
//			streamParams->waitTimer[TRIAD][k] = timer_end(timerStart,computeRank); // wait time for SCALE 
//		} 
//
//		/*
//		* TRIAD 
//		*/ 
//		timerStart = timer_start(computeRank); // start timing 
//		for(i = 0; i< iocompParams->localDataSize; i++)
//		{
//			a[i] = b[i] + c[i] * constant;  
//			mpiWaitFlag[ADD]=dataSendTest(iocompParams,&requestArray[ADD]); // test if ADD data got sent  
//		}
//		streamParams->compTimer[TRIAD][k] = timer_end(timerStart,computeRank); // computeTime for TRIAD
//		dataSend(a,iocompParams, &requestArray[TRIAD], streamParams->localDataSize); // send data
//		streamParams->sendTimer[TRIAD][k] = timer_end(timerStart,computeRank); // send time for TRIAD 
//#ifndef NDEBUG
//		for(i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",a[i]); }
//		printf("After TRIAD\n"); 
//#endif
	} // end avg loop  

	stopSend(iocompParams); // send ghost message to stop MPI_Recvs 
#ifndef NDEBUG
	printf("After stopSend function\n"); 
#endif

	if(computeRank == 0)
	{
		programEndTime = MPI_Wtime();
		wallTime = programEndTime - programStartTime; 
		//resultsOutput(streamParams->compTimer, streamParams->sendTimer, streamParams->waitTimer,wallTime); // write to csv file for compute write 
	} 

}
