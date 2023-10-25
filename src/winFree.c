#include "../include/iocomp.h"
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
