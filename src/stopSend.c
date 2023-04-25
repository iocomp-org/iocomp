
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

void stopSend(struct iocomp_params *iocompParams)
{
	int globalRank, ierr, compRank;  
	ierr = MPI_Comm_rank(iocompParams->globalComm, &globalRank);
	ierr = MPI_Comm_rank(iocompParams->compServerComm, &compRank);
	mpi_error_check(ierr); 

	if(iocompParams->hyperthreadFlag) // check if flag is true? 
	{
		/*
		 *	Send data using MPI_Isend to computeRank in interComm  
		 *	which is paired with the same rank of IO server 
		 */
		int dest, tag; 
		dest = getPair(iocompParams); // destination for sending data. 
		tag = globalRank; // tag should be rank of computeServer

		double ghost = 0.0; 

#ifdef VERBOSE_2
		printf("stopSend -> Sending data starts from compProcessor with globalRank %i to ioProcessor with globalRank  %i  \n", globalRank, dest); 
#endif
		ierr = MPI_Send(&ghost, 0 , MPI_DOUBLE, dest, tag,
				iocompParams->globalComm); // every rank sends its portion of data 
		mpi_error_check(ierr); 
#ifndef NDEBUG
		VERBOSE_1(compRank,"stopSend -> Ghost message sent \n"); 
#endif
	}
	/*
	 * if HT flag is off, then adios2 object should be finalised 
	 * as the ioServer finalises the adios2 object 
	 */ 
	else 
	{
		if(iocompParams->ioLibNum >=2 && iocompParams->ioLibNum <= 4)
		{
			adios2_finalize(iocompParams->adios); 
		} 
#ifndef NDEBUG
		VERBOSE_1(compRank,"stopSend -> adios2 finalised with HT flag=%i \n", iocompParams->hyperthreadFlag); 
#endif
		// delete file function is called in ioServer
		// for non HT implementation, files should be deleted after the data is sent
		// through 
#ifndef DNDelete
		if(compRank == 0)
		{
			deleteFiles(iocompParams); // delete files 
		} 
#endif
	}
} 

