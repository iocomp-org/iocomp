/** @file winInits.c
 *  @brief Initialises windows for shared memory interfaces and mallocs the pointers for the split case. 
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */
#include "../include/iocomp.h"
#include <inttypes.h>

/**
 * @file winInits.c
 * @brief Initialises windows for shared memory interfaces and mallocs the pointers for the split case.
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param localDataSize size of local data to be written
 * @return void
 */
void winInits(struct iocomp_params *iocompParams, int localDataSize)
{
	if(iocompParams->sharedFlag == true)
	{
#ifdef VERBOSE
		fprintf(iocompParams->debug,"winInits->shared flag true");
#endif
		/* 
		 * initialise window test flags with 0 
		 * assign them 1 if ready for writing 
		 */ 
		for(int i = 0; i < iocompParams->numWin; i++)
		{
			iocompParams->wintestflags[i] = 0; 
#ifdef VERBOSE
			fprintf(iocompParams->debug,"winInits->Wintest flags for window %i = %i \n", i, iocompParams->wintestflags[i]);
#endif
		} 
		int ierr; 
		// allocate shared windows 
		for(int i = 0; i < iocompParams->numWin; i++)
		{
			ierr = MPI_Win_allocate_shared(sizeof(double)*localDataSize, sizeof(double), MPI_INFO_NULL, iocompParams->newComm, &iocompParams->array[i], &iocompParams->winMap[i]); 
			mpi_error_check(ierr);
		}
#ifdef VERBOSE
		fprintf(iocompParams->debug,"winInits->after window shared allocated \n");
#endif
	}
	else
	{
#ifdef VERBOSE
		fprintf(iocompParams->debug,"winInits->shared flag not true \n ");
#endif
		/*
		 * otherwise malloc using the number of elements and double filesize  
		 */ 
		for(int i = 0; i < iocompParams->numWin; i++)
		{
			iocompParams->array[i] = (double*)malloc(localDataSize*sizeof(double)); 
			assert(iocompParams->array[i] != NULL); 
#ifdef VERBOSE
			fprintf(iocompParams->debug, "Memory address of array at %i 0x%" PRIXPTR "\n" ,i, (uintptr_t)iocompParams->array[i]);
#endif
		}
#ifdef VERBOSE
		fprintf(iocompParams->debug,"winInits->Arrays malloced \n");
#endif
	} 

} 
