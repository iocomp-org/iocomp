#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

int dataSendTest(struct iocomp_params *iocompParams,MPI_Request *request, double* array)
{
	if(iocompParams->hyperthreadFlag) // only implement if HT flag switched on 
	{
		int mpiWaitFlag; 
		int ierr = MPI_Test(request,&mpiWaitFlag,MPI_STATUS_IGNORE); // MPI test 
		mpi_error_check(ierr); 
		return(mpiWaitFlag);
	}
	else if(iocompParams->sharedFlag)
	{
		for(int i = 0; i < NUM_WIN; i++)
		{
			if(iocompParams->array[i] == array)
			{
				iocompParams->wintestflags[i] = WIN_TEST;  
			}
		} 
		return -1; 
	} 
	else
	{
		return -1; 
	} 
} 

