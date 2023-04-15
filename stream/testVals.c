#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"

void checkArray(struct stream_params* streamParams, double *readData, double* testData)
{
	printf("check array started \n"); 
	for(int i = 0; i< streamParams->globalDataSize; i++)
	{
		if(readData[i]!=testData[i])
		{
			printf("value of array not matching STREAM array at location %i \n", i); 
			exit(1); 
		}
	}
} 

void test_vals(struct iocomp_params *iocompParams, struct stream_params* streamParams, double *testData, char* KERNEL)
{	
	double* readData = NULL; 
	printf("Testing values for kernel %s \n", KERNEL);  

	switch(iocompParams->ioLibNum)
	{
		case 0: 
			readData = mpiRead(iocompParams); 
			checkArray(streamParams, readData, testData); 
			break; 

		case 1: 
			readData = hdf5Read(iocompParams); 
			checkArray(streamParams, readData, testData); 
			break; 

		case 2: case 3: case 4: 
			readData = adios2Read(iocompParams); 
			checkArray(streamParams, readData, testData); 
			break; 

		 default:
				break; 
	}

	free(readData); 
	readData = NULL; 
	
}
