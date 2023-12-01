/*
 * function gets shared array pointer using mpi win shared query 
 * and prints out data as proxy for writing to file 
 */

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>  
#include <string.h> 
#include <assert.h> 
#include <math.h>
#include "../include/iocomp.h"

void ioServer_shared(struct iocomp_params *iocompParams)
{
	
	// initialise IO Params structure 
	iocompParams->ioComm = iocompParams->ioServerComm; 
	int ioRank, ierr; 
	ierr = MPI_Comm_rank(iocompParams->ioComm, &ioRank); 
	mpi_error_check(ierr); 
#ifdef VERBOSE 
	fprintf(iocompParams->debug, "ioServer->IO server comm initialised IO rank %i\n", ioRank); 
#endif 

	// allocate shared windows and their pointers 
	ioServer_sharedAllocate(iocompParams); 

	// Allocate cartesian communicator, adios2 objects	
	ioServerInitialise(iocompParams); 

	arrayParamsInit(iocompParams, iocompParams->ioServerComm); 

	// declare mult variable to test for completion among all windows 

#ifdef IOBW
	for(int i = 0 ; i < iocompParams->numWin; i ++)
	{
		// initialise timers 
		for(int j = 0; j < AVGLOOPCOUNT; j++)
		{
			iocompParams->winTime[i][j] = 0.0; 
			iocompParams->writeTime[i][j] = 0.0; 
		}
	}
#endif 

	/* 
	 * initialise flag variable to test for window completion
	 * -1 means that window has not been initialised.
	 * 0 means that window has been initialised 
	 * 1 returned from win test means that the array has been written
	 */ 
	for(int i = 0; i< iocompParams->numWin; i++)
	{
		iocompParams->flag[i] = -1; 
	} 
#ifdef VERBOSE 
	fprintf(iocompParams->debug, "ioServer->flag initialised with -1\n"); 
#endif 

	for(;;)  
	{
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "ioServer->before MPI Bcast \n"); 
#endif 
		MPI_Bcast(iocompParams->wintestflags, iocompParams->numWin, MPI_INT, 0, iocompParams->newComm); 
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "ioServer -> after MPI bcast, iocompParams->wintestflags "); 
		for(int i = 0; i < iocompParams->numWin; i++)
		{
			fprintf(iocompParams->debug, "[%i]", 
					iocompParams->wintestflags[i]); 
		} 
		fprintf(iocompParams->debug, "\n"); 
#endif 

		for(int i = 0; i < iocompParams->numWin; i++)
		{
			if(iocompParams->wintestflags[i] > WIN_TEST) // anything over 0 means go for printing 
			{
				/* 
				 * if win wait implemented and test flag is still 0 i.e. file not ready
				 * yet then implement win wait 
				 */ 
				if(iocompParams->wintestflags[i]==WIN_WAIT && iocompParams->flag[i]==0)  
				{
					winWait(iocompParams, i); 
				}

				/*
				 * Obtain file name from preDataSend 
				 * Start window with WIN POST. 
				 * Initialise flag = 0 and start window timer 
				 * Test for window completion 
				 */ 
				if(iocompParams->wintestflags[i]==WIN_ACTIVATE)  
				{
					getFileName(iocompParams, i); 
					winPost(iocompParams, i); 
					winTest(iocompParams, i); 
				} 
			}

			/* 
			 * if previous winTest comes back negative and does not write out the file
			 */ 
			else if(iocompParams->wintestflags[i] == WIN_TEST && iocompParams->flag[i] == 0)
			{
				winTest(iocompParams, i); 
			} 
		}

		/*
		 * exit condition true if all flags are assigned win free 
		 */
		int exitcheck; 
		for(int j = 0; j < iocompParams->numWin; j++)
		{
			exitcheck = 0;  // reset value 
			if(iocompParams->wintestflags[j] == WIN_FREE)
			{
				exitcheck = 1; 
			}
		}

		if(exitcheck)
		{
			break; 
		}
	} 
#ifdef VERBOSE 
	fprintf(iocompParams->debug, "ioServerShared -> loop server exited \n"); 
#endif 

	/* 
	 * Clean up after loop server exit, wait for data to be obtained and then free
	 * the window 
	 */ 
	for(int i = 0; i < iocompParams->numWin; i++)
	{
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "ioServerShared -> flag value %i for window %i\n", iocompParams->flag[i], i); 
#endif 
		// wait for completion of all windows 
		if(iocompParams->flag[i] == 0)
		{
			winWait(iocompParams, i); 
		}

#ifdef VERBOSE 
		fprintf(iocompParams->debug, "ioServerShared-> MPI barrier activated\n"); 
#endif 
		// MPI_Barrier(iocompParams->newComm); // wait till each process is finished  
		winFree(iocompParams, i); 
	} 

#ifdef VERBOSE 
	fprintf(iocompParams->debug, "ioServerShared-> all windows freed \n"); 
#endif 
#ifndef NOADIOS2 
	// Finalise ADIOS2 object
	if(iocompParams->ioLibNum >=2 && iocompParams->ioLibNum <= 4)
	{
		adios2_finalize(iocompParams->adios);
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "ioServer->adios2 finalised\n"); 
#endif	
	} 
#endif 

#ifdef IOBW
	iobw(iocompParams); 
#endif 
} 

