#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"
#include "test.h"

#define iter 10 

void stream(double* iodata, struct iocomp_params *iocompParams)
{
#ifndef NDEBUG
	printf("stream starts\n"); 
#endif
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
	double timerStart, timerEnd, timer[4][iter]; 
	timerStart = timerEnd = 0.0; 
	for(k = 0; k< iter; k++) // averaging 
	{

#ifndef NDEBUG
		printf("After inits\n"); 
#endif
		// copy
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerStart = MPI_Wtime();
		}
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			c[i] = iodata[i]; 
		}
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			timer[0][k] = timerEnd - timerStart; 
		}

#ifndef NDEBUG
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
		}
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			timer[1][k] = timerEnd - timerStart; 
		}

#ifndef NDEBUG
		printf("After scale\n"); 
#endif

		// add 
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerStart = MPI_Wtime();
		}
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			c[i] = iodata[i] + b[i]; 
		}
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			timer[2][k] = timerEnd - timerStart; 
		}

#ifndef NDEBUG
		printf("After add\n"); 
#endif

		//triad 
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerStart = MPI_Wtime();
		}
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			a[i] = b[i] + iodata[i] * constant;  
		}
		if (computeRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timerEnd = MPI_Wtime();
			timer[3][k] = timerEnd - timerStart; 
		}
#ifndef NDEBUG
		printf("After triad\n"); 
#endif
	} 

	if(computeRank == 0)
	{
		double avgSum[4], sum; 
		for (i = 0; i<4; i++)
		{
			sum = 0;
			for (k = 0; k < iter; k++)
			{
				sum += timer[i][k]; 
			}
			avgSum[i] = sum/iter;
		}

		// printing
		printf("Copy  Scalar  Add  Triad  \n"); 
		printf("%lf  %lf  %lf  %lf \n", avgSum[0], avgSum[1], avgSum[2], avgSum[3]); 
	} 

}

//void avgTime(double* timer) 
//{
//	int i, k; 
//	double avgSum[4], sum; 
//	for (i = 0; i<4; i++)
//	{
//		sum = 0;
//		for (k = 0; k < iter; k++)
//		{
//			sum += avgTime[i][k]; 
//		}
//		avgSum[i] = sum/iter;
//	}
//	
//	// printing
//	printf("Copy  Scalar  Add  Triad  \n"); 
//	printf("%lf  %lf  %lf  %lf \n", avgSum[0], avgSum[1], avgSum[2], avgSum[3]); 
//
//}
void computeStep(double* iodata, struct iocomp_params *iocompParams)
{
	stream(iodata,iocompParams); 
#ifndef NDEBUG
	printf("After stream\n"); 
#endif
	computeServer(iodata,iocompParams); // send data off to computeServer 
#ifndef NDEBUG
	printf("After computeServer\n"); 
#endif
} 

