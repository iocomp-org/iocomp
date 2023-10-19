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

	// Probe for an incoming message from I/O process
	MPI_Probe(0, 0, MPI_COMM_WORLD, &status);

	// When probe returns, the status object has the size and other
	// attributes of the incoming message. Get the message size
	MPI_Get_count(&status, MPI_CHAR, &size);

	// Allocate a pointer to the double pointer 
	iocompParams->writeFile[i] = (char*)malloc(sizeof(char) * size);

	// Receive the filename message with the allocated buffer
	MPI_Recv(&iocompParams->writeFile[i], size, MPI_CHAR, 0, 0, iocompParams->newComm, MPI_STATUS_IGNORE); 
	printf("File name %s with size\n", iocompParams->writeFile[i], size);
} 

