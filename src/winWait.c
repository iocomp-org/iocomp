#include "../include/iocomp.h"
void winWait(struct iocomp_params *iocompParams, int windowNum)
{
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServer window:%i flag value:%i before win wait implemented\n", i, iocompParams->flag[i]); 
#endif 
	// wait for window completion 
	int ierr = MPI_Win_wait(iocompParams->winMap[i]); 
	mpi_error_check(ierr); 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServer window:%i flag value:%i after  win wait implemented\n", i, iocompParams->flag[i]); 
#endif 
	ioLibraries(iocompParams->array[i], iocompParams); 
	iocompParams->flag[i] = 1; 
} 
