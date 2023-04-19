#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

void dataWait(struct iocomp_params *iocompParams,MPI_Request *request)
{

	if(iocompParams->hyperthreadFlag) // only implement if HT flag switched on 
	{
		int compRank; 
		MPI_Comm_rank(iocompParams->compServerComm, &compRank); 
#ifndef NDEBUG
		VERBOSE_1(compRank,"dataWait -> MPI wait activated\n"); 
#endif
		// wait for the data to be sent from previous dataSend
		int ierr = MPI_Wait(request, MPI_STATUS_IGNORE);  
		mpi_error_check(ierr); 
	}

} 

