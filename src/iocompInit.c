#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

#define fullNode 256 

//void mpi_error_check(int ierr)
//{
//	if (ierr != MPI_SUCCESS)
//	{
//		printf("MPI call fails \n"); 
//	}
//}

/*
 * Initialises the library 
 */
MPI_Comm iocompInit(struct iocomp_params *iocompParams, MPI_Comm comm, bool FLAG, int ioLibNum)
{

#ifndef NDEBUG
	printf("iocompInit -> Start of intercomm_init\n"); 
#endif

	iocompParams->hyperthreadFlag = FLAG; // set hyperthread flag 
	/*
	 * arrayParamsInit initialises all the array datasize variables 
	 */ 

	//arrayParamsInit(iocompParams, comm, NDIM, localArraySize); 
#ifndef NDEBUG
	printf("iocompInit -> Array variables initialised\n"); 
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
	 * iocomp function that sends data to ioServer if flag is true
	 * if not then its a dead send 
	 */ 
	ioServerInitialise(iocompParams, ioLibNum); 

	/*
	 * if HT flag is on, iocomp function returns the compute comm 
	 * so that the user only works with the compute comm
	 * but if HT flag is off, and there is no splitting, then the world comm 
	 * is returned
	 */ 
	if(FLAG)
	{
		return(iocompParams->compServerComm); 
	}
	else
	{
		return(comm); 
	}

#ifndef NDEBUG
	printf("iocomp_init -> end of function\n"); 
#endif

} 

