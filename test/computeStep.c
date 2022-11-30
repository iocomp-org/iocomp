/* Inspired from the STREAM benchmark */ 
/* github url https://github.com/jeffhammond/STREAM.git */ 

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "test.h"

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
	ierr = MPI_Comm_rank(comm, &computeRank); 
	mpi_error_check(ierr); 
	double timerStart, timerEnd, timer[4][iter], totalTimer[4][iter]; 
	timerStart = timerEnd = 0.0; 
	MPI_Request request1, request2, request3, request4; 
	int mpiWaitFlag; 

#ifndef NDEBUG
	printf("After inits\n"); 
#endif

	for(k = 0; k< iter; k++) // averaging 
	{
		// copy
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerStart = MPI_Wtime();
		}
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			c[i] = a[i]; 
		}
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			timer[0][k] = timerEnd - timerStart; 
		}
		dataSend(c,iocompParams, &request1); // send data off using dataSend
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			totalTimer[0][k] = timerEnd - timerStart; 
		}
#ifndef NDEBUG
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			printf("%lf,",c[i]); 
		}
		printf("After copy\n"); 
#endif

		// scale 
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerStart = MPI_Wtime();
		}
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			b[i] = constant * c[i]; 
			MPI_Test(&request1,&mpiWaitFlag,MPI_STATUS_IGNORE); // MPI test insertion 
		}
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			timer[1][k] = timerEnd - timerStart; 
		}

		dataWait(iocompParams,&request1); // wait for previous data to be sent 
		dataSend(b,iocompParams, &request2); // send data off using dataSend
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			totalTimer[1][k] = timerEnd - timerStart; 
		}
#ifndef NDEBUG
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			printf("%lf,",b[i]); 
		}
		printf("After scale\n"); 
#endif

		// add 
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerStart = MPI_Wtime();
		}
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			c[i] = a[i] + b[i]; 
			MPI_Test(&request2,&mpiWaitFlag,MPI_STATUS_IGNORE); // MPI test insertion 
		}
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			timer[2][k] = timerEnd - timerStart; 
		}

		dataWait(iocompParams,&request2); // wait for previous data to be sent 
		dataSend(c,iocompParams, &request3); // send data off using dataSend
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			totalTimer[2][k] = timerEnd - timerStart; 
		}
#ifndef NDEBUG
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			printf("%lf,",c[i]); 
		}
		printf("After add\n"); 
#endif

		//triad 
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerStart = MPI_Wtime();
		}
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			a[i] = b[i] + c[i] * constant;  
			MPI_Test(&request3,&mpiWaitFlag,MPI_STATUS_IGNORE); // MPI test insertion 
		}
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			timer[3][k] = timerEnd - timerStart; 
		}

		dataWait(iocompParams,&request3); // wait for previous data to be sent 
		dataSend(a,iocompParams, &request4); // send data off using dataSend
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			totalTimer[3][k] = timerEnd - timerStart; 
		}
#ifndef NDEBUG
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			printf("%lf,",a[i]); 
		}
		printf("After triad\n"); 
#endif

		MPI_Test(&request4,&mpiWaitFlag,MPI_STATUS_IGNORE); // MPI test insertion 
		dataWait(iocompParams,&request4); // wait for previous data to be sent 

	} // end avg loop  

	stopSend(iocompParams); // send ghost message to stop MPI_Recvs 
#ifndef NDEBUG
	printf("After stopSend function\n"); 
#endif

	if(computeRank == 0)
	{
		resultsOutput(timer, totalTimer); // write to csv file for compute write 
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



