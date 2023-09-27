#include "../include/iocomp.h"
/*! \file winInits.c
	\brief Initialises windows for shared memory interfaces and otherwise. 

	Allocates shared memory interfaces for the shared memory option returning the shared MPI window and the array. 
	Otherwise the function will allocate the arrays with the required data size and return the array.  
	*/

void winInits(struct iocomp_params *iocompParams, int localDataSize)
{
	if(iocompParams->sharedFlag == true)
	{
#ifndef NDEBUG
		fprintf(iocompParams->debug,"winInits->shared flag true");
#endif
		/* 
		 * initialise window test flags with 0 
		 * assign them 1 if ready for writing 
		 */ 
		for(int i = 0; i < NUM_WIN; i++)
		{
			iocompParams->wintestflags[i] = 0; 
#ifndef NDEBUG
			fprintf(iocompParams->debug,"winInits->Wintest flags for window %i = %i \n", i, iocompParams->wintestflags[i]);
#endif
		} 
		int ierr; 
		// allocate shared windows 
		for(int i = 0; i < NUM_WIN; i++)
		{
			ierr = MPI_Win_allocate_shared(sizeof(double)*localDataSize, sizeof(double), MPI_INFO_NULL, iocompParams->newComm, &iocompParams->array[i], &iocompParams->winMap[i]); 
			mpi_error_check(ierr);
		}
#ifndef NDEBUG
		fprintf(iocompParams->debug,"winInits->after window shared allocated \n");
#endif
	}
	else
	{
#ifndef NDEBUG
		fprintf(iocompParams->debug,"winInits->shared flag not true \n ");
#endif
		/*
		 * otherwise malloc using the number of elements and double filesize  
		 */ 
		for(int i = 0; i < NUM_WIN; i++)
		{
			iocompParams->array[i] = (double*)malloc(localDataSize*sizeof(double)); 
			assert(iocompParams->array[i] != NULL); 
		}
#ifndef NDEBUG
		fprintf(iocompParams->debug,"winInits->Arrays malloced \n");
#endif
	} 

} 
