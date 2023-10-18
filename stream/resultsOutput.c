#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 
#include "stdio.h"
#include "mpi.h"
#include "stream.h"
#include "assert.h"
#define filename "compute_write_time.csv"

void averages(struct stream_params* streamParams)
{
	for (int i = 0; i<KERNELS; i++)
	{
		// initialise timers 
		streamParams->avgCompTimer[i]=0.0; 
		streamParams->avgWaitTimer[i]=0.0; 
		streamParams->avgSendTimer[i]=0.0; 
		
		// average out the values from compue values 
		int avgCount = AVGLOOPCOUNT/WRITE_FREQ; 
		for (int k = 0; k < AVGLOOPCOUNT; k++)
		{
			streamParams->avgCompTimer[i] += (double)(streamParams->maxCompTimer[i][k]/(double)avgCount); 
			streamParams->avgWaitTimer[i] += (double)(streamParams->maxWaitTimer[i][k]/(double)avgCount); 
			streamParams->avgSendTimer[i] += (double)streamParams->maxSendTimer[i][k]/(double)avgCount; 
		}
	}
} 

// reduce and maximise timers from all stream kernels 
void reduceResults(struct stream_params* streamParams,MPI_Comm computeComm)
{
	for(int i = 0; i < KERNELS; i++)
	{
		MPI_Reduce(&streamParams->compTimer[i],&streamParams->maxCompTimer[i],AVGLOOPCOUNT, MPI_DOUBLE, MPI_MAX, 0,computeComm); 
		MPI_Reduce(&streamParams->waitTimer[i],&streamParams->maxWaitTimer[i],AVGLOOPCOUNT, MPI_DOUBLE, MPI_MAX, 0,computeComm); 
		MPI_Reduce(&streamParams->sendTimer[i],&streamParams->maxSendTimer[i],AVGLOOPCOUNT, MPI_DOUBLE, MPI_MAX, 0,computeComm); 
	} 
	MPI_Reduce(&streamParams->wallTimer,&streamParams->maxWallTimer,1, MPI_DOUBLE, MPI_MAX, 0,computeComm); // wall time only 1 
} 

void resultsOutput(struct stream_params* streamParams, MPI_Comm computeComm)
{
		
	averages(streamParams); // get average timings from the reduced times 
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
	fprintf(out, "Compute,%lf,%lf,%lf,%lf \n", streamParams->avgCompTimer[0], streamParams->avgCompTimer[1], streamParams->avgCompTimer[2], streamParams->avgCompTimer[3]); 
	fprintf(out, "Send,%lf,%lf,%lf,%lf \n", streamParams->avgSendTimer[0], streamParams->avgSendTimer[1], streamParams->avgSendTimer[2], streamParams->avgSendTimer[3]); 
	fprintf(out, "Wait,%lf,%lf,%lf,%lf \n", streamParams->avgWaitTimer[0], streamParams->avgWaitTimer[1], streamParams->avgWaitTimer[2], streamParams->avgWaitTimer[3]); 
	fprintf(out, "WallTimer,,,,,%lf \n", streamParams->maxWallTimer); 
} 

// print out all the values to a different value per kernel, not just average values 
void fullResultsOutput(struct stream_params* streamParams)
{
	int test; 
	// define filenames 
	streamParams->fullResults_filename[0] = 		"copy.csv" ; 
	streamParams->fullResults_filename[1] = 		"scale.csv"; 
	streamParams->fullResults_filename[2] = 		"add.csv"		; 
	streamParams->fullResults_filename[3] = 		"triad.csv"; 
	
	// go via each kernel 
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
		fprintf(out, "Iter,CompTimer(s),WaitTimer(s),SendTimer(s)\n"); 
		int writeCounter = 0; // reset for every kernel 
		for (int j = 0; j < AVGLOOPCOUNT; j++)
		{
			fprintf(out, "%i, %lf,%lf,%lf \n", j, streamParams->maxCompTimer[i][j], streamParams->maxWaitTimer[i][j], streamParams->maxSendTimer[i][j]); 
		} 
	} 
}
