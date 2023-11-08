#include "../include/iocomp.h"
/**
 * @file winFree.c
 * @brief Frees windows after data transfer is complete.
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param i window number
 * @return void
 */
void winFree(struct iocomp_params *iocompParams, int i)
{
#ifdef VERBOSE 
	fprintf(iocompParams->debug, "ioServerShared-> window:%i before win free reached \n",i); 
#endif 
	int ierr = MPI_Win_free(&iocompParams->winMap[i]);
	mpi_error_check(ierr); 
#ifdef VERBOSE 
	fprintf(iocompParams->debug, "ioServerShared-> window:%i after win free \n",i); 
#endif 
}
