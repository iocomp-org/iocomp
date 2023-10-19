#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>  
#include <string.h> 
#include <assert.h> 
#include <math.h>
#include "../include/iocomp.h"

void getFileName(struct iocomp_params *iocompParams, int i)
{
	MPI_Status status;
	int size; 
	int ierr; 

	// Probe for an incoming message from I/O process
	ierr = MPI_Probe(0, 0, iocompParams->newComm, &status);
	mpi_error_check(ierr); 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "MPI probe \n"); 
#endif 
	
	// When probe returns, get size of filename 
	ierr = MPI_Get_count(&status, MPI_CHAR, &size);
	mpi_error_check(ierr); 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "MPI get count with size %i\n", size); 
#endif 

	// Allocate a pointer to the double pointer plus space for extensions  
	iocompParams->writeFile[i] = (char*)malloc(sizeof(char) * (size + 5));
	assert(iocompParams->writeFile[i]!=NULL); 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "getFileName -> MPI write file malloced  \n");
#endif 

	// Receive the filename message with the allocated buffer
	ierr = MPI_Recv(iocompParams->writeFile[i], size, MPI_CHAR, 0, 0, iocompParams->newComm, MPI_STATUS_IGNORE); 
	mpi_error_check(ierr); 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "File name %s with size %i for window number %i \n", iocompParams->writeFile[i], size, i );
#endif 
} 

