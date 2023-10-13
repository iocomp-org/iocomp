#include "../include/iocomp.h"
void winPost(struct iocomp_params *iocompParams, int i) 
{
	ierr = MPI_Win_post(iocompParams->group, 0, iocompParams->winMap[i]);
	mpi_error_check(ierr); 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServerShared->window %i after win post\n", i); 
#endif 
	iocompParams->flag[i] = 0; // window activated 
#ifdef IOBW	
	iocompParams->winTime_start[i] = MPI_Wtime();
#endif 
} 
