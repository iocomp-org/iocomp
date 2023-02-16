#include <stdlib.h>
#include <math.h>
#include "stdio.h"
#include "mpi.h"
#include "test.h"

#define IOLIBNUM 1
static int HT_flag;

double* initialise(struct iocomp_params* iocompParams, struct test_variables* testParams, int NDIM, int* localArraySize, MPI_Comm comm)
{

	// assign filenames to test structure 
	char* filenames[5] = {
		"mpiio.dat",
		"hdf5.h5",
		"adios2.h5",
		"adios2.bp4",
		"adios2.bp5"
	}; 
	testParams->FILENAMES[0] = "mpiio.dat"; 
	testParams->FILENAMES[1] = "hdf5.h5"; 
	testParams->FILENAMES[2] = "adios2.h5";
	testParams->FILENAMES[3] = "adios2.bp4";
	testParams->FILENAMES[4] = "adios2.bp5"; 
	
  // int localArraySize[NDIM] = {1000,1000}; 
  testParams->ioLibNum = IOLIBNUM; 
	size_t localDataSize = 1; 
	for(int i = 0; i < NDIM; i++)
	{
		localDataSize *= localArraySize[i]; 
	}

	testParams->localDataSize = localDataSize; 
	testParams->globalDataSize = localDataSize * testParams->mysize * pow(10,9); 
  double* data = NULL; // initialise data pointer  
  data = (double*)malloc(localDataSize*sizeof(double)); // one rank only sends to one rank

	// populate values of test array 
	for(int j=0; j < localDataSize; j++)
	{
		data[j] = (double)j*2; 
	}

	printf("Test -> localArraySize value %i %i \n", localArraySize[0], localArraySize[1]); 


	return data; 
}


