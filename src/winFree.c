#include "../include/iocomp.h"
void winFree(struct iocomp_params *iocompParams, int i)
{
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServerShared-> window:%i before win free reached, MPI barrier activated\n",i); 
#endif 
	MPI_Barrier(iocompParams->newComm); // wait till each process is finished  
	int ierr = MPI_Win_free(&iocompParams->winMap[i]);
	mpi_error_check(ierr); 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServerShared-> window:%i after win free \n",i); 
#endif 
}
