#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 
#include "stdio.h"
#include "mpi.h"
#include "stream.h"
#define filename "compute_write_time.csv"

double* avg(double data[KERNELS][10], struct stream_params* streamParams)
{
	int i,k;  
	double* sum = (double*)malloc(KERNELS*sizeof(double)); // one rank only sends to one rank
	for (i = 0; i<KERNELS; i++)
	{
		sum[i] = 0.0; 
		for (k = 0; k < streamParams->numWrites ; k++)
		{
			sum[i] += data[i][k]; 
		}
		sum[i] = sum[i]/streamParams->numWrites; 
	}
	return(sum); 
}

void reduceResults(struct stream_params* streamParams,MPI_Comm computeComm)
{
	// reduce and maximise timers from all stream kernels 
	for(int i = 0; i < KERNELS; i++)
	{
		MPI_Reduce(&streamParams->compTimer[i],&streamParams->maxCompTimer[i],LOOPCOUNT, MPI_DOUBLE, MPI_MAX, 0,computeComm); 
		MPI_Reduce(&streamParams->waitTimer[i],&streamParams->maxWaitTimer[i],LOOPCOUNT, MPI_DOUBLE, MPI_MAX, 0,computeComm); 
		MPI_Reduce(&streamParams->sendTimer[i],&streamParams->maxSendTimer[i],LOOPCOUNT, MPI_DOUBLE, MPI_MAX, 0,computeComm); 
	} 

	MPI_Reduce(&streamParams->wallTimer,&streamParams->maxWallTimer,1, MPI_DOUBLE, MPI_MAX, 0,computeComm); 
} 


void resultsOutput(struct stream_params* streamParams, MPI_Comm computeComm)
{
//	double avgCompTimer[KERNELS], 
//				 avgSendTimer[KERNELS], 
//				 avgWaitTimer[KERNELS]; 
	
	double* avgCompTimer = NULL; 
	double* avgSendTimer = NULL; 
	double* avgWaitTimer = NULL; 

	// calculate the averages from the max timers 
	avgCompTimer = avg(streamParams->maxCompTimer,streamParams); 
	avgSendTimer = avg(streamParams->maxSendTimer,streamParams); 
	avgWaitTimer = avg(streamParams->maxWaitTimer,streamParams); 

	// initialise the file variables 
	int test; 
	FILE* out; 
#ifndef NDEBUG
	printf("remove filename \n");
#endif
	test = remove(filename);
	if (test != 0)
	{
#ifndef NDEBUG
		printf("Cant remove %s \n", filename);
#endif 
	}
	out = fopen(filename, "w+");
	if (out == NULL)
	{
		printf("Error: No output file\n");
		exit(1);
	}

	// write to file
	fprintf(out, "Timer,Copy(s),Scalar(s),Add(s),Triad(s),Total(s)\n"); 
	fprintf(out, "Compute,%lf,%lf,%lf,%lf \n", avgCompTimer[0], avgCompTimer[1], avgCompTimer[2], avgCompTimer[3]); 
	fprintf(out, "Send,%lf,%lf,%lf,%lf \n", avgSendTimer[0], avgSendTimer[1], avgSendTimer[2], avgSendTimer[3]); 
	fprintf(out, "Wait,%lf,%lf,%lf,%lf \n", avgWaitTimer[0], avgWaitTimer[1], avgWaitTimer[2], avgWaitTimer[3]); 
	fprintf(out, "WallTimer,,,,,%lf \n", streamParams->maxWallTimer); 
} 

// print out all the values, not just average values 
void fullResultsOutput(struct stream_params* streamParams)
{

	int test; 
	// define filenames 
	streamParams->fullResults_filename[0] = 		"copy.csv" ; 
	streamParams->fullResults_filename[1] = 		"scale.csv"; 
	streamParams->fullResults_filename[2] = 		"add.csv"		; 
	streamParams->fullResults_filename[3] = 		"triad.csv"; 

	for (int i = 0; i< KERNELS; i++)
	{
		FILE* out; 
#ifndef NDEBUG
		printf("remove filename \n");
#endif
		test = remove(streamParams->fullResults_filename[i]);
		if (test != 0)
		{
#ifndef NDEBUG
			printf("Cant remove %s \n",streamParams->fullResults_filename[i]);
#endif 
		}
		out = fopen(streamParams->fullResults_filename[i], "w+");
		if (out == NULL)
		{
			printf("Error: No output file\n");
			exit(1);
		}

		// write to file
		fprintf(out, "Iter,CompTimer(s),SendTimer(s),WaitTimer(s)\n"); 
		for (int j = 0; j < LOOPCOUNT/streamParams->writeFreq; j++)
		{
			fprintf(out, "%i, %lf, %lf, %lf\n", j, streamParams->maxCompTimer[i][j], streamParams->maxSendTimer[i][j],streamParams->maxWaitTimer[i][j]); 
		} 
	} 

}
