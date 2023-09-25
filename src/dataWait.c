#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

void dataWait(struct iocomp_params *iocompParams,MPI_Request *request, double* array)
{

	if(iocompParams->hyperthreadFlag) // only implement if HT flag switched on 
	{
		int compRank; 
		MPI_Comm_rank(iocompParams->compServerComm, &compRank); 
#ifndef NDEBUG
		fprintf(ioParams->debug,"dataWait -> MPI wait activated\n"); 
#endif
		// wait for the data to be sent from previous dataSend
		int ierr = MPI_Wait(request, MPI_STATUS_IGNORE);  
		mpi_error_check(ierr); 
	}
	else if(iocompParams->sharedFlag)
	{
		// assign WIN WAIT to array window 
		for(int i = 0; i < NUM_WIN; i++)
		{
			if(iocompParams->array[i] == array)
			{
				iocompParams->wintestflags[i] = WIN_WAIT;  
			}
		} 
	}

} 

