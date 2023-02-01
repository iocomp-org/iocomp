#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 
#include "stdio.h"
#include "mpi.h"
#include "test.h" 
#define filename "compute_write_time.csv"

void avg(double sum[KERNELS], double data[KERNELS][iter])
{
  int i,k;  
	// double sum[KERNELS]; 
	for (i = 0; i<KERNELS; i++)
	{
		sum[i] = 0.0; 
		for (k = 0; k < iter; k++)
		{
			sum[i] += data[i][k]; 
		}
		sum[i] = sum[i]/iter; 
	}
	// return(sum); 
}

void resultsOutput(double timer[4][iter], double totalTimer[4][iter],double waitTimer[4][iter], double wallTimer)
{
	double avgCompTimer[KERNELS], 
	avgTotalTimer[KERNELS], 
	avgWaitTimer[KERNELS]; 

	int i,k; 

	// calculate the averages from the total time 
	avg(avgCompTimer, timer); 
	avg(avgTotalTimer, totalTimer); 
	avg(avgWaitTimer, waitTimer); 
	// avgTotalTimer = avg(totalTimer); 
	// avgWaitTimer = avg(waitTimer); 

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

	// printing
	fprintf(out, "Timer,Copy(s),Scalar(s),Add(s),Triad(s),Total(s)\n"); 
	fprintf(out, "Compute,%lf,%lf,%lf,%lf \n", avgCompTimer[0], avgCompTimer[1], avgCompTimer[2], avgCompTimer[3]); 
	fprintf(out, "Total,%lf,%lf,%lf,%lf \n", avgTotalTimer[0], avgTotalTimer[1], avgTotalTimer[2], avgTotalTimer[3]); 
	fprintf(out, "Wait,%lf,%lf,%lf,%lf \n", avgWaitTimer[0], avgWaitTimer[1], avgWaitTimer[2], avgWaitTimer[3]); 
	fprintf(out, "WallTimer,,,,%lf \n", wallTimer); 
} 

