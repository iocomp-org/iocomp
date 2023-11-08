#include "../include/iocomp.h"
void winTest(struct iocomp_params *iocompParams, int windowNum) 
{
	int ierr = MPI_Win_test(iocompParams->winMap[windowNum], &iocompParams->flag[windowNum]); 
	mpi_error_check(ierr);
	// if window is available to print then print and end timer 
	if(iocompParams->flag[windowNum])
	{
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "ioServerShared->window:%i after win test with flag:%i\n",windowNum, iocompParams->flag[windowNum]); 
#endif 
		ioLibraries(iocompParams->array[windowNum], iocompParams, windowNum); 
	}
}
