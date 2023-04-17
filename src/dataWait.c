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
		int ierr = MPI_Wait(request, MPI_STATUS_IGNORE); // wait for the data to be sent from previous dataSend 
		mpi_error_check(ierr); 
		int compRank; 
		MPI_Comm_rank(iocompParams->compServerComm, &compRank); 
#ifdef VERBOSE_2
		VERBOSE_1(compRank,"dataWait -> MPI wait activated\n"); 
#endif
	}

} 

