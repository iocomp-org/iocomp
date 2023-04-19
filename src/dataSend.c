#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

void dataSend(double* data, struct iocomp_params *iocompParams, MPI_Request *request, size_t localDataSize)
{

	int ierr; 
	int globalRank, compRank; 
	ierr = MPI_Comm_rank(iocompParams->globalComm, &globalRank);
	mpi_error_check(ierr); 
	ierr = MPI_Comm_rank(iocompParams->compServerComm, &compRank);
	mpi_error_check(ierr); 

	iocompParams->localDataSize = localDataSize; // assign size of local array 

	if(iocompParams->hyperthreadFlag) // check if flag is true? 
	{
		/*
		 *	Send data using MPI_Isend to computeRank in interComm  
		 *	which is paired with the same rank of IO server 
		 */

		int dest, tag; 
		dest = getPair(iocompParams); // destination for sending data. 
		tag = globalRank; // tag should be rank of computeServer

#ifndef NDEBUG
		VERBOSE_1(compRank,"dataSend -> Sending data starts from compProcessor with globalRank %i 
				to ioProcessor with globalRank  %i  \n", globalRank, dest); 
#endif

			ierr = MPI_Isend(data, iocompParams->localDataSize , MPI_DOUBLE, dest, tag,
					iocompParams->globalComm, request); // every rank sends its portion of data 
		mpi_error_check(ierr); 

#ifndef NDEBUG
		VERBOSE_1(compRank,"dataSend -> Sending data stop \n"); 
#endif
	}
	else
	{
		/*
		 * For HT off, IF not previously initialised OR if element count is different than 
		 * last time, then define array local size and other I/O parameters such as array 
		 * start, local size, global size using arrayParamsInit 
		 */ 
		iocompParams->localDataSize = localDataSize;
		if(!iocompParams->previousInit || iocompParams->previousCount!=(int)localDataSize)
		{
			arrayParamsInit(iocompParams,iocompParams->ioServerComm); 
			iocompParams->previousInit = 1; 
			iocompParams->previousCount = localDataSize; 
		}
#ifndef NDEBUG
		VERBOSE_1(compRank,"dataSend -> Hyperthread flag deactivated, go to ioLibraries with 
				localDataSize %ld \n", localDataSize); 
#endif
			ioLibraries(data,iocompParams); // otherwise go straight to writing using ioLibraries 
	}

} 

