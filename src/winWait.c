#include "../include/iocomp.h"
void winWait(struct iocomp_params *iocompParams, int windowNum)
{
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServer window:%i flag value:%i before win wait implemented\n", windowNum, iocompParams->flag[windowNum]); 
#endif 
	// wait for window completion 
	int ierr = MPI_Win_wait(iocompParams->winMap[windowNum]); 
	mpi_error_check(ierr); 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServer window:%i flag value:%i after win wait implemented\n", windowNum, iocompParams->flag[windowNum]); 
#endif 
	ioLibraries(iocompParams->array[windowNum], iocompParams, windowNum); 
	iocompParams->flag[windowNum] = 1; 
} 
