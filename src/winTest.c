#include "../include/iocomp.h"
void winTest(struct iocomp_params *iocompParams, int i) 
{
	int ierr = MPI_Win_test(iocompParams->winMap[i], &iocompParams->flag[i]); 
	mpi_error_check(ierr);
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServerShared->window:%i after win test with flag:%i\n",i, iocompParams->flag[i]); 
#endif 
	// if window is available to print then print and end timer 
	if(iocompParams->flag[i])
	{
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "ioServerShared->window:%i flag positive value:%i \n",i, iocompParams->flag[i]); 
#endif
		ioLibraries(iocompParams->array[i], iocompParams); 
	}
}
