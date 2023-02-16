#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"

double* init(struct stream_params* streamParams, double* a) 
{
	
#ifndef NDEBUG
	printf("stream -> After malloc \n"); 
#endif
	
	for(int i = 0; i < streamParams->localDataSize; i++)
	{
		a[i] = streamParams->localDataSize  + i; 
	}
#ifndef NDEBUG
	printf("stream -> a array defined \n"); 
#endif

	return(a); 
}
