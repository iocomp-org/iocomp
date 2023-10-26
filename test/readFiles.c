#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>  
#include <string.h> 
#include <assert.h> 
#include <math.h>
#include "test.h"

double* readFiles(struct test_params *testParams, struct iocomp_params *iocompParams, char* readFile)
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
	else if( (testParams->io == 2) || (testParams->io == 3) )
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
	else
	{
		printf("Invalid option. Quiting \n"); 
		exit(1); 
	}
	
	return(readData); 
} 
