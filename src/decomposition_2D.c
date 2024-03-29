#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "mpi.h"
#include "../include/iocomp.h"

void decomposition_2D(struct iocomp_params *iocompParams, MPI_Comm comm)
{
	int ioSize,ioRank,i, ierr; 
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
	localSizes_array[ioRank] = iocompParams->localDataSize;  
	// get all values from all ranks 
	ierr = MPI_Allreduce(MPI_IN_PLACE, localSizes_array, ioSize, MPI_UNSIGNED_LONG, MPI_SUM, iocompParams->ioServerComm); 
	mpi_error_check(ierr); 
	
	// sum of all local sizes
	iocompParams->globalDataSize = 0; 
	for(i = 0; i < ioSize; i++)
	{	
		iocompParams->globalDataSize += iocompParams->localDataSize; 
	} 

	/*
	 * Get global size 
	 * Sum array of local sizes to get total global size. 
	 * Decompose this global size into 2D to avoid int32 overflows. 
	 * Works based on closest sq root. 
	 * if not multiple then goes down the range checking for numbers below the sq
	 * root and checking if thats a factorial of the local size or not. 
	 */		
	if(!ioRank)
	{
		double power = (double)1/iocompParams->NDIM; 
		int root = (int)pow(iocompParams->globalDataSize,power);

		if(pow(root,iocompParams->NDIM) == iocompParams->globalDataSize)
		{
			// if its a perfect square 
			iocompParams->globalArray[0] = root; 
			iocompParams->globalArray[1] = root; 
		}
		else if(iocompParams->globalDataSize%root == 0) 
		{
			// if closest sq root is a perfect factorial 
			iocompParams->globalArray[1] = root; 
			iocompParams->globalArray[0] = iocompParams->globalDataSize/root; 
		}
		else
		{
			// or if closest sq root is not a perfect factorial 
			// then reduce the numbers and check for the closest factorials 
			for(int i = 1; i < root; i++)
			{
				if(iocompParams->globalDataSize%(root-i) == 0) 
				{
					iocompParams->globalArray[1] = root - i; 
					iocompParams->globalArray[0] = iocompParams->globalDataSize/(root-i); 
					break; 
				}
			}
		}
		assert( (iocompParams->globalArray[0]*iocompParams->globalArray[1]) == iocompParams->globalDataSize);
	} 

	// broadcast global array values to avoid different global values. 
	ierr = MPI_Bcast(iocompParams->globalArray, 2, MPI_UNSIGNED_LONG, 0, iocompParams->cartcomm); 
	mpi_error_check(ierr); 
#ifdef VERBOSE
	fprintf(iocompParams->debug,"arrayParamsInit -> local data size %ld, global data size %ld\n",
		iocompParams->localDataSize, iocompParams->globalDataSize); 
#endif

	/*
	 * Get local size by using global dimensions.
	 * If local size is greater than global inner dimension, then inner local
	 * dimension is same as global inner dimension. If not then its same as the
	 * local size. 
	 * Local outer dimension is the rounded up value of local size / global outer
	 * dimension. 
	 * (a[2][3] 2 is outer, 3 is inner dimension) 
	 */ 
	if(iocompParams->localDataSize > iocompParams->globalArray[1])
	{
		iocompParams->localArray[1] = iocompParams->globalArray[1]; 
		iocompParams->localArray[0] = (size_t)ceil(((double)iocompParams->localDataSize/(double)iocompParams->localArray[1])); 
	} 
	else
	{
		iocompParams->localArray[1] = iocompParams->localDataSize; 
		iocompParams->localArray[0] = 1; 
	} 

	/*
	 * Get array start points by summing local sizes till process rank. 
	 * Convert the sum to a point in the global array
	 */
	unsigned long int offset = 0; 
	for(i = 0; i < ioRank; i++)
	{
		offset += localSizes_array[i]; 
	}
	// * use cieling function on array start pushing them upwards. 
	iocompParams->arrayStart[0] = (size_t)ceil((double)offset/(double)iocompParams->globalArray[1]); 
	iocompParams->arrayStart[1] = 0;
	// * global array [0] is sum of all array starts[0] -> updated values. 
	ierr = MPI_Allreduce(&iocompParams->localArray[0], &iocompParams->globalArray[0], 1, MPI_UNSIGNED_LONG, MPI_SUM, iocompParams->cartcomm); 
	mpi_error_check(ierr); 

	// * old method.
	// iocompParams->arrayStart[0] = (int)(offset/iocompParams->globalArray[1]); 
	// iocompParams->arrayStart[1] = offset%iocompParams->globalArray[1];  	
	
	printf("Rank %i - global data size %li, local data size %li, globalArray:[%lu,%lu], localArray:	[%lu,%lu], arrayStart:	[%lu,%lu] \n",
		ioRank, iocompParams->globalDataSize, iocompParams->localDataSize, iocompParams->globalArray[0], iocompParams->globalArray[1], iocompParams->localArray[0], iocompParams->localArray[1], iocompParams->arrayStart[0],iocompParams->arrayStart[1]); 

#ifdef VERBOSE   
	fprintf(iocompParams->debug,"arrayParamsInit-> globalArray:[%li,%li] \n",iocompParams->globalArray[0], iocompParams->globalArray[1] ); 
	fprintf(iocompParams->debug,"arrayParamsInit-> localArray:[%li,%li] \n", iocompParams->localArray[0],  iocompParams->localArray[1] ); 
	fprintf(iocompParams->debug,"arrayParamsInit-> startArray:[%li,%li] \n", iocompParams->arrayStart[0],  iocompParams->arrayStart[1] ); 
#endif 
} 
