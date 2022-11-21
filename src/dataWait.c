#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

void dataWait(struct iocomp_params *iocompParams,MPI_Request *request)
{

	int i, ierr; 

	if(iocompParams->hyperthreadFlag) // only implement if HT flag switched on 
	{
		MPI_Wait(request, MPI_STATUS_IGNORE); // wait for the data to be sent from previous dataSend  
#ifndef NDEBUG
		printf("MPI wait activated\n"); 
#endif
	}

} 

