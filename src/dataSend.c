#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

void dataSend(double* data, struct iocomp_params *iocompParams, MPI_Request *request)
{

	int i, ierr; 

	if(iocompParams->hyperthreadFlag) // check if flag is true? 
	{
		int globalRank; 
		ierr = MPI_Comm_rank(iocompParams->globalComm, &globalRank);
		mpi_error_check(ierr); 

		/*
		 *	Send data using MPI_Isend to computeRank in interComm  
		 *	which is paired with the same rank of IO server 
		 */

		int dest, tag; 
		dest = getPair(iocompParams); // destination for sending data. 
		tag = globalRank; // tag should be rank of computeServer

#ifndef NDEBUG
		printf("Sending data starts from compProcessor with globalRank %i to ioProcessor with globalRank  %i  \n", globalRank, dest); 
#endif
		// ierr = MPI_Send(data, iocompParams->localDataSize , iocompParams->dataType, dest, tag,
		//		iocompParams->globalComm); // every rank sends its portion of data 
		
		ierr = MPI_Isend(data, iocompParams->localDataSize , iocompParams->dataType, dest, tag,
					iocompParams->globalComm, request); // every rank sends its portion of data 
		mpi_error_check(ierr); 

#ifndef NDEBUG
		printf("Sending data stop \n"); 
#endif
	}
	else
	{
		ioLibraries(data,iocompParams); // otherwise go straight to writing 
	}

} 

