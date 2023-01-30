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
void ioServerInitialise(struct iocomp_params *iocompParams)
{
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
		} 
	} 

} 

