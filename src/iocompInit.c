#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

#define compColour 1
#define ioColour 0
#define fullNode 256 
#define HIGH_LOW 1
#define FIXED_IO_RANK 0

/*
 * Initialises the library 
 */
void iocompInit(struct iocomp_params *iocompParams, MPI_Comm comm, int NDIM, int* localArraySize)
{

#ifndef NDEBUG
	printf("Start of intercomm_init\n"); 
#endif

	arrayParamsInit(iocompParams, comm, NDIM, localArraySize); 

#ifndef NDEBUG
		printf("Array variables initialised\n"); 
#endif

	/*
	 * Intercomm created linking computeComm and ioComm to globalComm via
	 * interComm
	 */ 

	intercomm_create(iocompParams); 

#ifndef NDEBUG
	printf("End of iocomp_init\n"); 
#endif

} 

