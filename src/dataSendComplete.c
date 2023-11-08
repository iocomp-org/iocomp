/** @file dataSendComplete.c
 *  @brief Implements data send completion for async I/O.
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */
#include "../include/iocomp.h"

/**
 * @file dataSendComplete.c
 * @brief Calls MPI_Win_free for the corresponding array window.
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param array pointer to array to wait for
 * @return void
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

