#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

#define NDIM 2 
static int verbose_flag;
static int HT_flag; 

double* initialise(struct iocomp_params* iocompParams, MPI_Comm comm)
{

	// command line stuff can go here.. 

  // check for HT flag 
	HT_flag = 1; 
  if (HT_flag)
    puts ("HT flag is set to on");
  else 
    puts ("HT flag is switched off"); 

  int localArraySize[NDIM] = {4,4}; 
	size_t localDataSize = 1; 
	for(int i = 0; i < NDIM; i++)
	{
		localDataSize *= localArraySize[i]; 
	}

  double* data = NULL; // initialise data pointer  
  data = (double*)malloc(localDataSize*sizeof(double)); // one rank only sends to one rank

	// populate values of test array 
	for(int j=0; j < localDataSize; j++)
	{
		data[j] = (double)j*2; 
	}

  iocompInit(iocompParams, comm,  NDIM, localArraySize, HT_flag); 

	arrayParamsInit(iocompParams,comm,NDIM,localArraySize);

	return data; 
}


