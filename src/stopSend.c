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

		ierr = MPI_Send(&ghost, 0 , MPI_DOUBLE, dest, tag,
				iocompParams->globalComm); // every rank sends its portion of data 
		mpi_error_check(ierr); 
#ifndef NDEBUG
		fprintf(iocompParams->debug,"stopSend -> Ghost message sent \n"); 
#endif
	}
	else if(iocompParams->sharedFlag)
	{
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "stopSend->Before MPI windows freed\n"); 
#endif
		for(int i = 0; i < NUM_WIN; i++)
		{
			int ierr = MPI_Win_free(&iocompParams->winMap[i]);
			mpi_error_check(ierr); 
		} 
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "stopSend->MPI windows freed\n"); 
#endif
	} 
	/*
	 * if HT flag is off, then adios2 object should be finalised 
	 * as the ioServer finalises the adios2 object 
	 */ 
	else 
	{
#ifndef NOADIOS2
		if(iocompParams->ioLibNum >=2 && iocompParams->ioLibNum <= 4)
		{
			adios2_finalize(iocompParams->adios); 
		} 
#endif 

#ifndef NDEBUG
		fprintf(iocompParams->debug,"stopSend -> adios2 finalised with HT flag=%i \n", iocompParams->hyperthreadFlag); 
#endif
		// delete file function is called in ioServer
		// for non HT implementation, files should be deleted after the data is sent
		// through 
#ifndef NDELETE 
		MPI_Barrier(iocompParams->compServerComm); 
		if(compRank == 0)
		{
			deleteFiles(iocompParams); // delete files 
		} 
#endif
	}
} 

