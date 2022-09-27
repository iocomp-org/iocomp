#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

#define ioColour 0 
#define compColour 1 

void comm_split(struct iocomp_params *iocompParams)
{
			int ierr, myrank;
			MPI_Comm splitComm; 
			MPI_Comm_rank(iocompParams->globalComm, &myrank); 
			ierr =	MPI_Comm_split(iocompParams->globalComm, iocompParams->colour, myrank,	&splitComm);  // splitcommunicator based on colour 
			mpi_error_check(ierr);

#ifndef NDEBUG
			printf("MPI comm split \n"); 
#endif
			//if (iocompParams->colour == compColour) 
			//{	
				ierr = MPI_Comm_dup(splitComm, &iocompParams->compServerComm); // compute communicator for compute tasks and colour != 0 
				mpi_error_check(ierr); 

#ifndef NDEBUG
			printf("MPI compServerComm initialised \n"); 
#endif
			// }
			
			//if (iocompParams->colour == ioColour) 
			//{	
				ierr = MPI_Comm_dup(splitComm, &iocompParams->ioServerComm); // compute communicator for compute tasks and colour != 0 
				mpi_error_check(ierr); 

#ifndef NDEBUG
			printf("MPI ioServerComm initialised \n"); 
#endif
			//}
}

