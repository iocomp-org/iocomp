#include "../include/iocomp.h"
/*
 * function calls mpi barrier to sync up with io process 
 * so that it finishes accessing data and frees up window 
 */ 
void dataSendComplete(struct iocomp_params *iocompParams, double* array)
{
	// free window and free shared memory pointer 
#ifdef VERBOSE 
	fprintf(iocompParams->debug, "MPI win free comp server reached\n"); 
#endif

	for(int i = 0; i < iocompParams->numWin; i++)
	{
		if(iocompParams->array[i] == array)
		{
			int ierr = MPI_Win_free(&iocompParams->winMap[i]);
			mpi_error_check(ierr); 
		}
	} 
} 

