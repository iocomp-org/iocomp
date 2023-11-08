#include "../include/iocomp.h"
/**
 * @file winPost.c
 * @brief Posts windows for data transfer.
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param i window number
 * @return void
 */
void winPost(struct iocomp_params *iocompParams, int i) 
{
	int ierr = MPI_Win_post(iocompParams->group, 0, iocompParams->winMap[i]);
	mpi_error_check(ierr); 
#ifdef VERBOSE 
	fprintf(iocompParams->debug, "ioServerShared->window %i after win post\n", i); 
#endif 
	iocompParams->flag[i] = 0; // window activated 
#ifdef IOBW	
	iocompParams->winTime_start[i] = MPI_Wtime();
#endif 
} 
