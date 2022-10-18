#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 
#include "stdio.h"
#include "mpi.h"
#include "test.h" 
#define filename "compute_write_time.csv"

void resultsOutput(double timer[4][iter], double totalTimer[4][iter])
{
	double avgSum[4], sum, avgTotalSum[4], totalSum; 
	int i,k; 

	// calculate the averages from the total time 
	for (i = 0; i<4; i++)
	{
		sum = 0.0;
		totalSum = 0.0; 
		for (k = 0; k < iter; k++)
		{
			sum += timer[i][k]; 
			totalSum += totalTimer[i][k];  
		}
		avgSum[i] = sum/iter;
		avgTotalSum[i] = totalSum/iter; 
	}

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
	fprintf(out, "Timer					Copy(s)		Scalar(s)		Add(s)  Triad(s) \n"); 
	fprintf(out, "Compute Time	%lf				%lf					%lf			%lf \n", avgSum[0], avgSum[1], avgSum[2], avgSum[3]); 
	fprintf(out, "Total Time		%lf				%lf					%lf  		%lf \n", avgTotalSum[0], avgTotalSum[1], avgTotalSum[2], avgTotalSum[3]); 
} 

