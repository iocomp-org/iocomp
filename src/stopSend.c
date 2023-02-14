
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

void stopSend(struct iocomp_params *iocompParams)
{

	int i, ierr; 

#ifndef NDEBUG
		printf("stopSend -> starts \n"); 
#endif
	
	if(iocompParams->hyperthreadFlag) // check if flag is true? 
	{
#ifndef NDEBUG
		printf("stopSend -> HT flag \n"); 
#endif
		int globalRank; 
		ierr = MPI_Comm_rank(iocompParams->globalComm, &globalRank);
		mpi_error_check(ierr); 
		MPI_Request request;

#ifndef NDEBUG
		printf("stopSend -> globalRank \n"); 
#endif
		/*
		 *	Send data using MPI_Isend to computeRank in interComm  
		 *	which is paired with the same rank of IO server 
		 */

		int dest, tag; 
		dest = getPair(iocompParams); // destination for sending data. 
		tag = globalRank; // tag should be rank of computeServer

		double ghost = 0.0; 

#ifndef NDEBUG
		printf("stopSend -> Sending data starts from compProcessor with globalRank %i to ioProcessor with globalRank  %i  \n", globalRank, dest); 
#endif
		ierr = MPI_Send(&ghost, 0 , MPI_DOUBLE, dest, tag,
				iocompParams->globalComm); // every rank sends its portion of data 
		
		//ierr = MPI_Isend(data, iocompParams->localDataSize , iocompParams->dataType, dest, tag,
		//			iocompParams->interComm, &request); // every rank sends its portion of data 
		mpi_error_check(ierr); 

#ifndef NDEBUG
		printf("stopSend -> Sending data stop \n"); 
#endif
	}

} 

