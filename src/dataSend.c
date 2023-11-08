/** @file dataSend.c	
 *  @brief Implements sending data to I/O process. 
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

/**
 * @file dataSend.c
 * @brief Implements sending data to I/O process. 
 * if HT flag is true, then MPI_Isend is used to send data to I/O process.
 * if shared flag is true, then win complete is issued for the array window.
 * if both flags are false, then ioLibraries is called directly.
 * @param data pointer to array to send
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param request pointer to MPI_Request object
 * @param localDataSize size of array to send
 * @return void
 */
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

#ifdef VERBOSE
		fprintf(iocompParams->debug,"dataSend -> Sending data starts from compProcessor with globalRank %i\
				to ioProcessor with globalRank  %i  \n", globalRank, dest); 
#endif

			ierr = MPI_Isend(data, iocompParams->localDataSize , MPI_DOUBLE, dest, tag,
					iocompParams->globalComm, request); // every rank sends its portion of data 
		mpi_error_check(ierr); 

#ifdef VERBOSE
		fprintf(iocompParams->debug,"dataSend -> Sending data stop \n"); 
#endif
	}
	else if(iocompParams->sharedFlag)
	{
		// get the window test flag for the array 
		for(int i = 0; i < iocompParams->numWin; i++)
		{
			if(iocompParams->array[i] == data)
			{
				// match array with windows and issue win complete 
				MPI_Win_complete(iocompParams->winMap[i]); 
			}
		}
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
#ifdef VERBOSE
		fprintf(iocompParams->debug,"dataSend -> Hyperthread flag deactivated, go to ioLibraries with\
				localDataSize %ld \n", localDataSize); 
#endif
			ioLibraries(data,iocompParams, 0); // otherwise go straight to writing using ioLibraries 
	}

} 

