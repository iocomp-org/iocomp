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
void iocompInit(struct iocomp_params *iocompParams, MPI_Comm comm, int NDIM, int* localArraySize, bool FLAG)
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
	 * iocomp function that sends data to ioServer if flag is true
	 * if not then its a dead send 
	 */ 
	//ioServerInitialise(iocompParams); 
	/*
	if(iocompParams->hyperthreadFlag)
	{
		if(iocompParams->colour == ioColour)
		{
			ioServer(iocompParams);
#ifndef NDEBUG
			printf("After ioServer\n"); 
#endif
			MPI_Finalize(); 

#ifndef NDEBUG
			printf("After finalize\n"); 
#endif
			exit(0); 
#ifndef NDEBUG
			printf("After exit\n"); 
#endif
		} 
	} 
	*/ 

#ifndef NDEBUG
	printf("End of intercomm\n"); 
#endif

#ifndef NDEBUG
	printf("End of iocomp_init\n"); 
#endif

} 

