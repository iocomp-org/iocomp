#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 
#include "stdio.h"
#include "mpi.h"
#include "stream.h"
#define filename "compute_write_time.csv"

void avg(double sum[KERNELS], double data[KERNELS][LOOPCOUNT])
{
	int i,k;  
	for (i = 0; i<KERNELS; i++)
	{
		sum[i] = 0.0; 
		for (k = 0; k < LOOPCOUNT; k++)
		{
			sum[i] += data[i][k]; 
		}
		sum[i] = sum[i]/LOOPCOUNT; 
	}
	// return(sum); 
}

void resultsOutput(struct stream_params* streamParams)
{
	double avgCompTimer[KERNELS], 
				 avgSendTimer[KERNELS], 
				 avgWaitTimer[KERNELS]; 

	// calculate the averages from the total time 
	avg(avgCompTimer, streamParams->compTimer); 
	avg(avgSendTimer, streamParams->sendTimer); 
	avg(avgWaitTimer, streamParams->waitTimer); 

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
	fprintf(out, "WallTimer,,,,%lf \n", streamParams->wallTimer); 
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
		for (int j = 0; j < LOOPCOUNT; j++)
		{
			fprintf(out, "%i, %lf, %lf, %lf\n", j, streamParams->compTimer[i][j], streamParams->sendTimer[i][j],streamParams->waitTimer[i][j]); 
		} 
	} 

}
