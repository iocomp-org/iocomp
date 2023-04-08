#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 
#include "stdio.h"
#include "mpi.h"
#include "stream.h"
#define filename "compute_write_time.csv"

void averages(struct stream_params* streamParams)
{
	for (int i = 0; i<KERNELS; i++)
	{
		double sum_comp, sum_wait, sum_send; 
		sum_comp = 0.0; 
		sum_wait = 0.0; 
		sum_send = 0.0; 
		// waits and comps run for LOOPCOUNT times 
		for (int k = 0; k < LOOPCOUNT; k++)
		{
			sum_comp += (double)(streamParams->maxCompTimer[i][k]/(double)MAXWRITES); 
			sum_wait += (double)(streamParams->maxWaitTimer[i][k]/(double)MAXWRITES); 
		}
		// sends run for MAXWRITES times only
		for (int k = 0; k < MAXWRITES; k++)
		{
			sum_send += (double)streamParams->maxSendTimer[i][k]/(double)MAXWRITES; 
		}
	
		streamParams->avgWaitTimer[i] = sum_wait; 
		streamParams->avgCompTimer[i] = sum_comp; 
		streamParams->avgSendTimer[i] = sum_send; 
	}
} 

// reduce and maximise timers from all stream kernels 
void reduceResults(struct stream_params* streamParams,MPI_Comm computeComm)
{
	for(int i = 0; i < KERNELS; i++)
	{
		MPI_Reduce(&streamParams->compTimer[i],&streamParams->maxCompTimer[i],LOOPCOUNT, MPI_DOUBLE, MPI_MAX, 0,computeComm); 
		MPI_Reduce(&streamParams->waitTimer[i],&streamParams->maxWaitTimer[i],LOOPCOUNT, MPI_DOUBLE, MPI_MAX, 0,computeComm); 
		MPI_Reduce(&streamParams->sendTimer[i],&streamParams->maxSendTimer[i],MAXWRITES, MPI_DOUBLE, MPI_MAX, 0,computeComm); // max writes could differ from loop count
	} 

	MPI_Reduce(&streamParams->wallTimer,&streamParams->maxWallTimer,1, MPI_DOUBLE, MPI_MAX, 0,computeComm); 
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
		fprintf(out, "Iter,CompTimer(s),WaitTimer(s),SendTimer(s)\n"); 
		int writeCounter = 0; // reset for every kernel 
		for (int j = 0; j < LOOPCOUNT; j++)
		{
			if(j%streamParams->writeFreq==0) // if writing then write out send timer 
			{
				fprintf(out, "%i, %lf, %lf, %lf\n", j, streamParams->maxCompTimer[i][j], streamParams->maxWaitTimer[i][j], streamParams->maxSendTimer[i][writeCounter]); 
				writeCounter ++; 
			}
			else //else leave blank
			{
				fprintf(out, "%i, %lf,%lf,\n", j, streamParams->maxCompTimer[i][j],streamParams->maxWaitTimer[i][j]); 
			} 
		} 
	} 

}
