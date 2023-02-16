#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"

void init(struct stream_params* streamParams, double* a, double* b, double* c) 
{
	
  a = (double*)malloc(streamParams->localDataSize*sizeof(double)); // one rank only sends to one rank
  b = (double*)malloc(streamParams->localDataSize*sizeof(double)); // one rank only sends to one rank
  c = (double*)malloc(streamParams->localDataSize*sizeof(double)); // one rank only sends to one rank
#ifndef NDEBUG
	printf("After malloc \n"); 
#endif
	
	for(int i = 0; i < streamParams->localDataSize; i++)
	{
		a[i] = streamParams->localDataSize  + i; 
	}
}
