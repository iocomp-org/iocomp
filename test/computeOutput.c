#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 
#include "stdio.h"
#include "mpi.h"
#define filename "compute_write_time.csv"

void dataOutput(double avgSum[4])
{
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
		fprintf(out, "Copy(s)  Scalar(s)  Add(s)  Triad(s) \n"); 
		fprintf(out, "%lf  %lf  %lf  %lf \n", avgSum[0], avgSum[1], avgSum[2], avgSum[3]); 
} 
	
