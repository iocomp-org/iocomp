#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

void sendFileName(struct iocomp_params *iocompParams, char* fileName)
{
	// after sending data asynchronously, then send the file name in blocking
	// way 
	int ierr; 
	int globalRank; 
	int dest, tag; 
	dest = getPair(iocompParams); // destination for sending data. 
	// tag = globalRank; // tag should be rank of computeServer
	tag = 5; // tag should be rank of computeServer
	ierr = MPI_Comm_rank(iocompParams->globalComm, &globalRank);
	mpi_error_check(ierr); 

	ierr = MPI_Send(fileName, ((int)strlen(fileName)+1), MPI_CHAR, dest, tag, iocompParams->globalComm);  
	mpi_error_check(ierr); 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "preDataSend->file name sent %s with length %i\n", fileName, ((int)strlen(fileName)+1)) ; 
#endif 

} 
