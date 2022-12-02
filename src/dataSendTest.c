#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

void dataSendTest(struct iocomp_params *iocompParams,MPI_Request *request)
{

	int i, ierr; 
	int mpiWaitFlag; 

	if(iocompParams->hyperthreadFlag) // only implement if HT flag switched on 
	{
			MPI_Test(request,&mpiWaitFlag,MPI_STATUS_IGNORE); // MPI test 
#ifndef NDEBUG
		printf("MPI test\n"); 
#endif
	}

} 

