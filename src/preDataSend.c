#include "../include/iocomp.h"
#include "assert.h" 

void preDataSend(struct iocomp_params *iocompParams, double* array)
{

	if(iocompParams->sharedFlag)
	{
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "preDataSend -> Before MPI bcast, iocompParams->wintestflags"); 
		for(int i = 0; i < NUM_WIN; i++)
		{
			fprintf(iocompParams->debug, "[%i]", 
					iocompParams->wintestflags[i]); 
		} 
		fprintf(iocompParams->debug, "\n"); 
#endif 

		// check if test flags have a value between -1 and 4 before broadcasting 
		for(int i = 0; i < NUM_WIN; i++)
		{
			assert(iocompParams->wintestflags[i] > -2); 
			assert(iocompParams->wintestflags[i] < 5); 
		}

		// broadcast them to ioserver 
		MPI_Bcast( iocompParams->wintestflags, NUM_WIN, MPI_INT, 0, iocompParams->newComm); 
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "preDataSend -> After MPI broadcast  \n"); 
#endif 

		// get the window test flag for the array 
		for(int i = 0; i < NUM_WIN; i++)
		{
			if(iocompParams->array[i] == array)
			{
				// match array with windows and issue win complete 
#ifndef NDEBUG 
				fprintf(iocompParams->debug, "preDataSend-> Before mpi win start for window %i \n", i); 
#endif 
				int ierr = MPI_Win_start(iocompParams->group, 0, iocompParams->winMap[i]); 
				mpi_error_check(ierr); 
#ifndef NDEBUG 
				fprintf(iocompParams->debug, "preDataSend-> After mpi win start for window %i \n", i); 
#endif 
			}
		}
	}
} 

