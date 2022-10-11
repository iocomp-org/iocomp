#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

#define ioColour 0 
#define compColour 1 

void mpi_error_check(int ierr)
{
	if (ierr != MPI_SUCCESS)
	{
		printf("MPI call fails \n"); 
	}
}

void intercomm( struct iocomp_params *iocompParams )
{
	if(iocompParams->hyperthreadFlag == true)
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
}
