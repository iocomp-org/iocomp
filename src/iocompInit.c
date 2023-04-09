#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

/*
 * Initialises the library 
 */
MPI_Comm iocompInit(struct iocomp_params *iocompParams, MPI_Comm comm, bool FLAG, int ioLibNum, int fullNode)
{

#ifndef NDEBUG
	printf("iocompInit -> Start of intercomm_init\n"); 
#endif

	iocompParams->hyperthreadFlag = FLAG; // set hyperthread flag 
	iocompParams->NDIM = NUM_DIM; // set number of dimensions
	iocompParams->ioLibNum = ioLibNum; // set selection of I/O library 
	iocompParams->NODESIZE = fullNode; // set size of node for comm splitting 

	/*
	 * assert tests for input parameters 
	 */ 
	assert(iocompParams->ioLibNum < ioLibCount); 
	assert(iocompParams->ioLibNum >= 0);
	assert(iocompParams->fullNode > 0); 

#ifndef NDEBUG
	printf("iocompInit -> variables declared flag %i, ndim %i, iolib %i\n", iocompParams->hyperthreadFlag, iocompParams->NDIM, iocompParams->ioLibNum); 
#endif
	/*
	 * comm split splits communicators in 2, assigns colour to ranks
	 * assigns communicators in struct for both cases  
	 */ 
	comm_split(iocompParams, comm); 
#ifndef NDEBUG
	printf("iocompInit -> communicator split up and colour assigned \n"); 
#endif

	/*
	 * If HT flag is on, then called by io server, if HT flag off then called by
	 * every process. This is because only ioServers have access to ioServerComm.
	 * ioServerInitialise initialises cartesian communicator and adios2 objects,
	 * and sets up other I/O related variables.
	 */ 
	if(!iocompParams->hyperthreadFlag || iocompParams->colour==ioColour) 
	{
		ioServerInitialise(iocompParams); 
	} 

	/*
	 * If HT flag is on and process is ioserver then ioServer recieves data
	 * and finalises adios2 object & mpi finalizes and program exits. 
	 */ 
	if(iocompParams->hyperthreadFlag && iocompParams->colour == ioColour)
	{
#ifndef NDEBUG
			printf("ioServerInitialise -> ioServer called\n"); 
#endif
			ioServer(iocompParams);
#ifndef NDEBUG
			printf("ioServerInitialise -> After ioServer\n"); 
#endif
			MPI_Finalize(); 
#ifndef NDEBUG
			printf("ioServerInitialise -> After finalize\n"); 
#endif
			exit(0); 
	}

	/*
	 * Return comp server comm to compute processes if HT flag is on
	 * but if HT flag is off, and there is no splitting, then the world comm 
	 * is returned
	 */ 
	return(iocompParams->compServerComm); 
#ifndef NDEBUG
	printf("iocomp_init -> end of function\n"); 
#endif
} 

