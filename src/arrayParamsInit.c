#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

#define HIGH_LOW 1
#define FIXED_IO_RANK 0

/*
 * Initialises the iocomp_params struct 
 */
void arrayParamsInit(struct iocomp_params *iocompParams, MPI_Comm comm )
{

	int ioSize,ioRank, i; 
	MPI_Comm_size(comm, &ioSize);
	MPI_Comm_rank(comm, &ioRank);

	/*
	 * Array size specs initialise
	 */ 
	iocompParams->localArray = malloc(sizeof(size_t)*iocompParams->NDIM);
	iocompParams->globalArray = malloc(sizeof(size_t)*iocompParams->NDIM);
	iocompParams->dataType = MPI_DOUBLE; // data type of sent and recvd data 
#ifndef NDEBUG
	fprintf(iocompParams->debug,"arrayParamsInit -> local, global size initialised \n"); 
#endif

	/*
	 * Local and global sizes are initialised 
	 * localSize is decomposed into NDIM i.e. 2D array for now. 
	 * works based on closest sq root. 
	 * if not multiple then goes down the range checking for numbers below the sq
	 * root and checking if thats a factorial of the local size or not. 
	 */ 
	float power = (float)1/iocompParams->NDIM; 
	int root; 
	size_t dim[2] = {0,0}; // setting this to be 2 dimensions   
	root = (int)pow(iocompParams->localDataSize,power);

	// if its a perfect square 
	if(pow(root,iocompParams->NDIM) == iocompParams->localDataSize)
	{
		dim[0] = root; 
		dim[1] = root; 
	}

	// if closest sq root is a perfect factorial 
	else if(iocompParams->localDataSize%root == 0) 
	{
		dim[0] = root; 
		dim[1] = iocompParams->localDataSize/root; 
	}
	// or if closest sq root is not a perfect factorial 
	// then reduce the numbers and check for the closest factorials 
	else if(iocompParams->localDataSize%root != 0)
	{
		for(int i = 1; i < root; i++)
		{
			if(iocompParams->localDataSize%(root-i) == 0) 
			{
				dim[0] = root - i; 
				dim[1] = iocompParams->localDataSize/(root-i); 
				break; 
			}
		}
	}
	for (i = 0; i < iocompParams->NDIM; i++)
	{
		iocompParams->localArray[i] = dim[i]; 
		iocompParams->globalArray[i]	= iocompParams->localArray[i]; 
	}

	// final check if the array dimensions multiply to give the local data size 
	// size_t mult = (iocompParams->localArray[0]*iocompParams->localArray[1]); 
	// print("value of multiplication %li and localdatasize %li \n ", mult, iocompParams->localDataSize); 
	assert( (iocompParams->localArray[0]*iocompParams->localArray[1]) == iocompParams->localDataSize);

	/*
	 * globalArray is multiplied by total size of available processors 
	 * since comm is dependant on hyperthread flag, if HT flag is ON, global size is half 
	 * no need for if statements 
	 */ 
	iocompParams->globalArray[0]*= ioSize; // assumes outermost dimension gets expanded by each rank 


#ifndef NDEBUG   
	fprintf(iocompParams->debug,"arrayParamsInit-> globalArray:[%li,%li] \n",iocompParams->globalArray[0], iocompParams->globalArray[1] ); 
	fprintf(iocompParams->debug,"arrayParamsInit-> localArray:[%li,%li] \n", iocompParams->localArray[0],  iocompParams->localArray[1] ); 
	// fprintf(iocompParams->debug,"arrayParamsInit-> startArray:[%li,%li] \n", iocompParams->arrayStart[0],  iocompParams->arrayStart[1] ); 
#endif 

	/*
	 * Array local, global data sizes initialise 
	 */ 
	iocompParams->globalDataSize = iocompParams->localDataSize * ioSize; 

#ifndef NDEBUG
	fprintf(iocompParams->debug,"arrayParamsInit -> size definitions, localDataSize %li, globalDataSize %li\n", iocompParams->localDataSize, iocompParams->globalDataSize); 
#endif

	/*
	 * Define and initialise arrayStart
	 */ 
	iocompParams->arrayStart = malloc(sizeof(size_t)*iocompParams->NDIM);
#ifndef NDEBUG
	fprintf(iocompParams->debug,"arrayParamsInit -> initialise arrayStart \n");
#endif

	for (int i = 0; i < iocompParams->NDIM; i++)
	{
		iocompParams->arrayStart[i] = 0; 
	}
	iocompParams->arrayStart[0] = ioRank * iocompParams->localArray[0]; // assuming ar_size has uniform dimensions. 
#ifndef NDEBUG
	fprintf(iocompParams->debug,"arrayParamsInit -> arrayStart initialised ioRank = %i\n",ioRank);
#endif

} 
