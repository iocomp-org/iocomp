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
		if(k>0)
		{
			add_wait(iocompParams, streamParams, k); // wait for copy to be send its data 
		} 

		/*
		* COPY
		*/ 
		copy(iocompParams, streamParams, k, c, a); // send copy data and get timers for send and compute  
		scale_wait(iocompParams, streamParams, k); // wait for send to be finished sending its data 
		
		/*
		* SCALE
		*/ 
		scale(iocompParams, streamParams, k, c, b); // send scale data and get timers for send and compute  
		copy_wait(iocompParams, streamParams, k); // wait for copy to be send its data 

		/*
		* ADD
		*/ 
		add(iocompParams, streamParams, k, c, a, b); // send copy data and get timers for send and compute  
		triad_wait(iocompParams, streamParams, k); // wait for copy to be send its data 

		/*
		* TRIAD 
		*/ 
		triad(iocompParams, streamParams, k, c, a, b); // send copy data and get timers for send and compute  

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
