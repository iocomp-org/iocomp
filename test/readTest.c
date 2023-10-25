#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>  
#include <string.h> 
#include <assert.h> 
#include <math.h>
#include "test.h"

int valueCheck(struct iocomp_params *iocompParams, double* iodata_test, double val, char* filename)
{
	int test = 1; 
	for(int i = 0; i < iocompParams->localDataSize; i++)
	{
		if(iodata_test[i] != val)
		{
			printf("Verification failed for index %i for filename %s, read data=%lf, correct val=%lf \n", 
					i, filename, iodata_test[i], val); 
			test = 0; 
			break; 
		}
	} 
	return(test); 
} 

double* readTest(struct test_params *testParams, struct iocomp_params *iocompParams, char* readFile)
{ 
	// initialise data buffer to store data read locally 
	double* readData; 
	readData = (double *) malloc(sizeof(double)*testParams->localDataSize); 
	assert(readData!=NULL); 

	// different IO libraries called depending on IO parameter 
	if(testParams->io == 0)
	{
		if(file_exists(readFile)) 
		{
			mpiRead(readData, readFile, iocompParams); 
		} 
		else
		{
			return(NULL); 
		}
	} 
	else if(testParams->io == 1)
	{
		if(file_exists(readFile)) 
		{
			phdf5Read(readData, readFile, iocompParams); 
		} 
		else
		{
			return(NULL); 
		}
	} 
	else if( (testParams->io == 2) || (testParams->io == 3) || (testParams->io == 4))
	{
		if(file_exists(readFile)) 
		{
			adios2Read(readData, readFile, iocompParams); 
		} 
		else
		{
			return(NULL); 
		}
	} 
	
	return(readData); 
} 
