#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

#define fullNode 256 
#define FLAG true

void mpi_error_check(int ierr)
{
	if (ierr != MPI_SUCCESS)
	{
		printf("MPI call fails \n"); 
	}
}

/*
 * Initialises the library 
 */
void iocompInit(struct iocomp_params *iocompParams, MPI_Comm comm, int NDIM, int* localArraySize)
{

#ifndef NDEBUG
	printf("Start of intercomm_init\n"); 
#endif

	iocompParams->hyperthreadFlag = FLAG; // set hyperthread flag 
	/*
	 * arrayParamsInit initialises all the array datasize variables 
	 */ 

	arrayParamsInit(iocompParams, comm, NDIM, localArraySize); 
#ifndef NDEBUG
	printf("Array variables initialised\n"); 
#endif

	/*
	 * comm split splits communicators in 2, assigns colour to ranks
	 * assigns communicators in struct for both cases  
	 */ 

	comm_split(iocompParams, comm); 
#ifndef NDEBUG
	printf("communicator split up and colour assigned \n"); 
#endif

	/*
	 * Intercomm created linking computeComm and ioComm to globalComm via
	 * interComm
	 */ 

	if(iocompParams->hyperthreadFlag) // check if flag is true? 
	{
		intercomm_create(iocompParams); 
	} 
#ifndef NDEBUG
	printf("End of intercomm_create\n"); 
#endif

	/*
	 * Intercomm function that sends data to ioServer if flag is true
	 * if not then its a dead send 
	 */ 

	if(iocompParams->hyperthreadFlag)
	{
		if(iocompParams->colour == ioColour)
		{
			ioServer(iocompParams);
#ifndef NDEBUG
			printf("After ioServer\n"); 
#endif
			MPI_Finalize(); 
			exit(0); 
		} 
	} 
	else
	{
		printf("intercomm not created \n"); 
	}
#ifndef NDEBUG
	printf("End of intercomm\n"); 
#endif

#ifndef NDEBUG
	printf("End of iocomp_init\n"); 
#endif

} 

