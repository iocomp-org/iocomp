/** @file stopSend.c
 *  @brief Contains function for stopping send of data to I/O server. 
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

/**
 * @file stopSend.c
 * @brief Contains function for stopping send of data to I/O server. 
 * For HT mode, sends a ghost message to the paired rank in the interComm. 
 * This ghost message is a signal to the I/O server to stop receiving data by sending 
 * a message with length 0.
 * For shared mode, assigns win free to the wintestflags and calls MPI free
 * For sequential mode, it finalises the adios2 object and prints the timers for I/O times.
 * @param iocompParams pointer to struct containing all parameters for the library
 * @return void
 */
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
#ifdef VERBOSE
		fprintf(iocompParams->debug,"stopSend -> Ghost message sent \n"); 
#endif
	}
	else if(iocompParams->sharedFlag)
	{

		// send win free message to control arrays 
		for(int i = 0; i < iocompParams->numWin; i++)
		{
			iocompParams->wintestflags[i] = WIN_FREE;  
		}
		dataSendInfo(iocompParams);

#ifdef VERBOSE
		fprintf(iocompParams->debug,"stopSend -> Barrier reached before Win free called \n"); 
#endif
		// MPI_Barrier(iocompParams->newComm); // wait till each process is free  
		for(int i = 0; i < iocompParams->numWin; i++)
		{
#ifdef VERBOSE 
			fprintf(iocompParams->debug, "stopSend->window:%i Before win free\n", i); 
#endif
			int ierr = MPI_Win_free(&iocompParams->winMap[i]);
			mpi_error_check(ierr); 
#ifdef VERBOSE 
			fprintf(iocompParams->debug, "stopSend->window:%i After win free\n", i); 
#endif
		} 
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

#ifdef VERBOSE
		fprintf(iocompParams->debug,"stopSend -> adios2 finalised with HT flag=%i \n", iocompParams->hyperthreadFlag); 
#endif
		/* write timers to file after reducing across ranks*/
#ifndef NOTIMERS
		printWriteTimers(iocompParams); 
#ifdef VERBOSE
		fprintf(iocompParams->debug,"stopSend -> print write timers\n"); 
#endif
#endif
	}
} 

