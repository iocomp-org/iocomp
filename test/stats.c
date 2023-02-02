#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "test.h"
#define OUT_FILE "output.csv"
void stats(struct test_variables* testParams)
{
	// remove output file if exists. 
	rmfile(OUT_FILE); 

	// declare file variable open in append mode 
	FILE* fp; 	
	fp = fopen(OUT_FILE,"a"); 
	if(fp == NULL) {
			printf("file couldn't be opened\n");
			exit(1);
	}

	// headers for csv file 	
	fprintf(fp, "FileName, ProcessSize, GlobalDataSize(GB), Iteration, WriteTime(s) \n"); 

	double writeTime = 0.0; 
	double globalDataSize = 0.0; 

	// write write timers per iteration 
	if(!testParams->myrank)
	{
		for(int i = 0; i < LOOPCOUNT; i++)
		{
			writeTime = testParams->endTime[i] - testParams->startTime[i]; 
			globalDataSize = (double) testParams->localDataSize * testParams->mysize * sizeof(double) / (pow(10,9)); 
			fprintf(fp,"%s, %i, %lf, %i, %lf \n", testParams->FILENAMES[testParams->ioLibNum], testParams->mysize, globalDataSize, i, writeTime  ) ; 
		}
	} 
	fclose(fp); 
} 
