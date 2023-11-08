#include "../include/iocomp.h"
void ioServer_sharedAllocate(struct iocomp_params *iocompParams)
{
	int soi = sizeof(double); 

	// allocate shared windows 
	for(int i = 0; i < iocompParams->numWin; i++)
	{
		int ierr = MPI_Win_allocate_shared(0, soi, MPI_INFO_NULL, iocompParams->newComm, &iocompParams->array[i], &iocompParams->winMap[i]); 
		mpi_error_check(ierr);
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "ioServer -> MPI allocated window %i \n", i); 
#endif 
	}

	// allocate arrays using window pointers 
	for(int i = 0; i < iocompParams->numWin; i++)
	{
		long int arraySize; 
		int dispUnit; 
		int ierr; 
		ierr = MPI_Win_shared_query(iocompParams->winMap[i], 0, &arraySize, &dispUnit, &iocompParams->array[i]); 
		mpi_error_check(ierr); 
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "ioServer -> MPI shared query for window %i arraysize is %li \n", i, arraySize); 
#endif 
		iocompParams->localDataSize = arraySize/sizeof(double); // mpi shared query gives the size in bytes assuming every window is writing the same amount of data  
	} 

	for(int i = 0; i < iocompParams->numWin; i++)
	{
		iocompParams->wintestflags[i] = 0; 
	} 
#ifdef VERBOSE 
	fprintf(iocompParams->debug, "ioServer -> after win test flags initialised\n"); 
#endif 
} 

