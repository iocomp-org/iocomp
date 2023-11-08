#include "../include/iocomp.h"
/**
 * @file winTest.c
 * @brief Tests if window is available for printing.
 * If so then IO libraries is called. 
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param windowNum window number
 * @return void
 */
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
