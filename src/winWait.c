#include "../include/iocomp.h"
/**
 * @file winWait.c
 * @brief Waits for window to be ready for writing.
 * Then calls IO libraries to write data to file and assigns test flag to 1.
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param windowNum window number
 * @return void
 */
void winWait(struct iocomp_params *iocompParams, int windowNum)
{
#ifdef VERBOSE 
	fprintf(iocompParams->debug, "ioServer window:%i flag value:%i before win wait implemented\n", windowNum, iocompParams->flag[windowNum]); 
#endif 
	// wait for window completion 
	int ierr = MPI_Win_wait(iocompParams->winMap[windowNum]); 
	mpi_error_check(ierr); 
#ifdef VERBOSE 
	fprintf(iocompParams->debug, "ioServer window:%i flag value:%i after win wait implemented\n", windowNum, iocompParams->flag[windowNum]); 
#endif 
	ioLibraries(iocompParams->array[windowNum], iocompParams, windowNum); 
	iocompParams->flag[windowNum] = 1; 
} 
