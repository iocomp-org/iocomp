#include "../include/iocomp.h"
/*! \file winInits.c
	\brief Initialises windows for shared memory interfaces and otherwise. 

	Allocates shared memory interfaces for the shared memory option returning the shared MPI window and the array. 
	Otherwise the function will allocate the arrays with the required data size and return the array.  
	*/

void winInits(struct iocomp_params *iocompParams, int localDataSize)
{
	if(iocompParams->sharedFlag == 1)
	{
		/* 
		 * initialise window test flags with 0 
		 * assign them 1 if ready for writing 
		 */ 
		for(int i = 0; i < NUM_WIN; i++)
		{
			iocompParams->wintestflags[i] = 0; 
		} 

		int ierr; 
		// allocate shared windows 
		for(int i = 0; i < NUM_WIN; i++)
		{
			ierr = MPI_Win_allocate_shared(sizeof(double)*localDataSize, sizeof(double), MPI_INFO_NULL, iocompParams->newComm, &iocompParams->array[i], &iocompParams->winMap[i]); 
			mpi_error_check(ierr);
		}
		// allocate groups 
		/*
		 * groups newComm communicator's rank 0 and 1 into a group
		 * comp process initialises array and creates a window with that array
		 */
		MPI_Group comm_group;
		int ranks[2];
		for (int i=0;i<2;i++) {
			ranks[i] = i;     //For forming groups, later
		}
		MPI_Comm_group(iocompParams->newComm,&comm_group);

		/* I/O group consists of ranks 1 */
		MPI_Group_incl(comm_group,1,ranks+1,&iocompParams->group);

		// initialise wintestflags 
		for(int j = 0; j < NUM_WIN; j++)
		{
			iocompParams->wintestflags[j] = 0;
		}
	}
	else
	{
		/*
		 * otherwise malloc using the number of elements and double filesize  
		 */ 
		for(int i = 0; i < NUM_WIN; i++)
		{
			iocompParams->array[i] = (double*)malloc(localDataSize*sizeof(double)); 
			assert(iocompParams->array[i] != NULL); 
		}
	} 

} 
