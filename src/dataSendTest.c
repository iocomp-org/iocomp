#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

int dataSendTest(struct iocomp_params *iocompParams,MPI_Request *request)
{


	if(iocompParams->hyperthreadFlag) // only implement if HT flag switched on 
	{
			int mpiWaitFlag; 
			int ierr = MPI_Test(request,&mpiWaitFlag,MPI_STATUS_IGNORE); // MPI test 
			mpi_error_check(ierr); 
#ifndef NDEBUG
		printf("MPI test\n"); 
#endif
			return(mpiWaitFlag);
	}
	else
		return -1; 

} 

