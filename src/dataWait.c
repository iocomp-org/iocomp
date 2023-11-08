#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

void dataWait(struct iocomp_params *iocompParams,MPI_Request *request, double* array, char* fileName)
{

	if(iocompParams->hyperthreadFlag) // only implement if HT flag switched on 
	{
		int compRank; 
		MPI_Comm_rank(iocompParams->compServerComm, &compRank); 
#ifdef VERBOSE
		fprintf(iocompParams->debug,"dataWait -> MPI wait activated\n"); 
#endif
		// wait for the data to be sent from previous dataSend
		int ierr = MPI_Wait(request, MPI_STATUS_IGNORE);  
		mpi_error_check(ierr); 
		
		// send file name for the data after array has been sent 
		sendFileName(iocompParams, fileName); 
	}
	else if(iocompParams->sharedFlag)
	{
		// assign WIN WAIT to array window 
		for(int i = 0; i < iocompParams->numWin; i++)
		{
			if(iocompParams->array[i] == array)
			{
				iocompParams->wintestflags[i] = WIN_WAIT;  
			}
		} 
	}

} 

