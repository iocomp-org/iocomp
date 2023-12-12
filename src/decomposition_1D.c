#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "mpi.h"
#include "../include/iocomp.h"

// Get 1D data decomposition
void decomposition_1D(struct iocomp_params *iocompParams, MPI_Comm comm)
{
	int ioSize,ioRank,i;  
	MPI_Comm_size(comm, &ioSize);
	MPI_Comm_rank(comm, &ioRank);

	/*
	 * Get all local datasizes from all the ranks. 
	 * Assign process rank with size of its data. 
	 * Everything else is 0 
	 * Then get all ranks to fill all the values of the array.
	 */ 
	unsigned long int localSizes_array[ioSize]; // to comply with MPI data types 
	for(i = 0; i < ioSize; i++)
	{
		localSizes_array[i] = 0; 
	}
	localSizes_array[ioRank] = (unsigned long int)iocompParams->localDataSize;  
	// get all values from all ranks 
	MPI_Allreduce(MPI_IN_PLACE, localSizes_array, ioSize, MPI_UNSIGNED_LONG, MPI_SUM, iocompParams->ioServerComm); 

	// sum of all local sizes
	iocompParams->globalDataSize = 0; 
	for(i = 0; i < ioSize; i++)
	{	
		iocompParams->globalDataSize += localSizes_array[i]; 
	} 

	// get offset by calculating sum of all sizes before process rank 
	iocompParams->arrayStart[0] = 0; 
	for(i = 0; i < ioRank; i++)
	{
		iocompParams->arrayStart[0] += (size_t)localSizes_array[i];  
	} 

	// local and global array parameters are same as local and global data sizes. 
	iocompParams->localArray[0] = iocompParams->localDataSize;
	iocompParams->globalArray[0] = iocompParams->globalDataSize; 
  if(!ioRank)
  {
	  printf("Global data size %li \n", iocompParams->globalDataSize); 
  }

#ifdef VERBOSE   
	fprintf(iocompParams->debug,"Rank %i - global data size %li, local data size %li, globalArray:[%li], localArray:[%li], arrayStart:[%li] \n",
			ioRank, iocompParams->globalDataSize, iocompParams->localDataSize, 
			iocompParams->globalArray[0], iocompParams->localArray[0], iocompParams->arrayStart[0]); 
#endif 

} 
