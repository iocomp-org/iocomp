#include <stdlib.h>
#include <math.h>
#include "stdio.h"
#include "mpi.h"
#include "test.h"

double* initialise(struct test_variables* testParams)
{
	// assign filenames to test structure 
	testParams->FILENAMES[0] = "mpiio.dat"; 
	testParams->FILENAMES[1] = "hdf5.h5"; 
	testParams->FILENAMES[2] = "adios2.h5";
	testParams->FILENAMES[3] = "adios2.bp4";
	testParams->FILENAMES[4] = "adios2.bp5"; 

  double* data = NULL; // initialise data pointer  
  data = (double*)malloc(testParams->localDataSize*sizeof(double)); // one rank only sends to one rank

	// populate values of test array 
	for(int j=0; j < testParams->localDataSize; j++)
	{
		//data[j] = (double)j*2; 
		data[j] = 1; 
	}

	return data; 
}


