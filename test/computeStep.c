/* Inspired from the STREAM benchmark */ 
/* github url https://github.com/jeffhammond/STREAM.git */ 

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"
#include "test.h"

#define iter 10 

void stream(double* data, struct iocomp_params *iocompParams)
{
#ifndef NDEBUG
	printf("stream starts\n"); 
#endif
	if (data == NULL)
	{
		printf("data is null \n"); 
		return; 
	}
	int i, ierr,k; 
	int constant = 5; 
	double c[iocompParams->localDataSize]; 
	double b[iocompParams->localDataSize]; 
	double a[iocompParams->localDataSize]; 
	int computeRank;
	MPI_Comm comm; 
	comm = iocompParams->globalComm; 
	ierr = MPI_Comm_rank(comm, &computeRank); 
	mpi_error_check(ierr); 
	double timerStart, timerEnd, timer[4][iter], totalTimer[4][iter]; 
	timerStart = timerEnd = 0.0; 

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
			c[i] = data[i]; 
		}
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			timer[0][k] = timerEnd - timerStart; 
		}
		computeServer(data,iocompParams); // send data off using computeServer
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			totalTimer[0][k] = timerEnd - timerStart; 
		}
	} 

#ifndef NDEBUG
		printf("After copy\n"); 
#endif

	for(k = 0; k< iter; k++) // averaging 
	{
		// scale 
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerStart = MPI_Wtime();
		}
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			b[i] = constant * c[i]; 
		}
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			timer[1][k] = timerEnd - timerStart; 
		}

		computeServer(data,iocompParams); // send data off using computeServer
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			totalTimer[1][k] = timerEnd - timerStart; 
		}
	} 
#ifndef NDEBUG
		printf("After scale\n"); 
#endif

	for(k = 0; k< iter; k++) // averaging 
	{
		// add 
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerStart = MPI_Wtime();
		}
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			c[i] = data[i] + b[i]; 
		}
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			timer[2][k] = timerEnd - timerStart; 
		}
		
		computeServer(data,iocompParams); // send data off using computeServer
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			totalTimer[2][k] = timerEnd - timerStart; 
		}
	} 

#ifndef NDEBUG
		printf("After add\n"); 
#endif

	for(k = 0; k< iter; k++) // averaging 
	{
		//triad 
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerStart = MPI_Wtime();
		}
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			a[i] = b[i] + data[i] * constant;  
		}
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			timer[3][k] = timerEnd - timerStart; 
		}
		
		computeServer(data,iocompParams); // send data off using computeServer
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			totalTimer[3][k] = timerEnd - timerStart; 
		}
	} 
#ifndef NDEBUG
		printf("After triad\n"); 
#endif
	
	if(computeRank == 0)
	{
		dataOutput(timer, totalTimer); // write to csv file for compute write 
	} 

}

void computeStep(double* data, struct iocomp_params *iocompParams)
{
	//data = (double*)malloc(iocompParams->localDataSize*sizeof(double)); // one rank only sends to one rank
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



