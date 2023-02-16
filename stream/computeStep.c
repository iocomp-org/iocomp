/* Inspired from the STREAM benchmark */ 
/* github url https://github.com/jeffhammond/STREAM.git */ 

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"


void computeStep(struct iocomp_params *iocompParams, struct stream_params *streamParams, MPI_Comm comm)
{
#ifndef NDEBUG
	printf("Starting computeStep \n"); 
#endif
	int i, globalMPIRank, ierr;   

  double* data = NULL; // initialise data pointer  
  data = (double*)malloc(streamParams->localDataSize*sizeof(double)); // one rank only sends to one rank
#ifndef NDEBUG
	printf("After malloc \n"); 
#endif

	ierr = MPI_Comm_rank(comm, &globalMPIRank); 
	printf("global mpi rank %i \n",globalMPIRank); 
	mpi_error_check(ierr); 

	for(i = 0; i < streamParams->localDataSize; i++)
	{
		data[i] = streamParams->localDataSize  + i; 
	}
	// stream(data,iocompParams, streamParams, comm); 
	stopSend(iocompParams); 
#ifndef NDEBUG
	printf("After stream\n"); 
#endif

  free(data); 
  data = NULL; 
} 



