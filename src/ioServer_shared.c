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


void winWait(struct iocomp_params *iocompParams, int i)
{
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServer window:%i flag value:%i before win wait implemented\n", i, iocompParams->flag[i]); 
#endif 
	// wait for window completion 
	ierr = MPI_Win_wait(iocompParams->winMap[i]); 
	mpi_error_check(ierr); 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServer window:%i flag value:%i after  win wait implemented\n", i, iocompParams->flag[i]); 
#endif 
	ioLibraries(iocompParams->array[i], iocompParams); 
	iocompParams->flag[i] = 1; 
} 

void winTest(struct iocomp_params *iocompParams, int i) 
{
	ierr = MPI_Win_test(iocompParams->winMap[i], &iocompParams->flag[i]); 
	mpi_error_check(ierr);
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServerShared->window:%i after win test with flag:%i\n",i, iocompParams->flag[i]); 
#endif 
	// if window is available to print then print and end timer 
	if(iocompParams->flag[i])
	{
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "ioServerShared->window:%i flag positive value:%i \n",i, iocompParams->flag[i]); 
#endif
		ioLibraries(iocompParams->array[i], iocompParams); 
	}

}

void winPost(struct iocomp_params *iocompParams, int i) 
{
	ierr = MPI_Win_post(iocompParams->group, 0, iocompParams->winMap[i]);
	mpi_error_check(ierr); 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServerShared->window %i after win post\n", i); 
#endif 
	iocompParams->flag[i] = 0; // window activated 
#ifdef IOBW	
	iocompParams->winTime_start[i] = MPI_Wtime();
#endif 

} 

void winFree(struct iocomp_params *iocompParams, int i)
{
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServerShared-> window:%i before win free reached\n",i); 
#endif 
	ierr = MPI_Win_free(&iocompParams->winMap[i]);
	mpi_error_check(ierr); 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServerShared-> window:%i after win free \n",i); 
#endif 
}


void ioServer_shared(struct iocomp_params *iocompParams)
{

	// initialise IO Params structure 
	iocompParams->ioComm = iocompParams->ioServerComm; 
	int ioRank, ierr; 
	ierr = MPI_Comm_rank(iocompParams->ioComm, &ioRank); 
	mpi_error_check(ierr); 

	// allocate shared windows and their pointers 
	ioServer_sharedAllocate(iocompParams); 

	// Allocate cartesian communicator, adios2 objects	
	ioServerInitialise(iocompParams); 

	arrayParamsInit(iocompParams, iocompParams->ioServerComm); 

	// declare mult variable to test for completion among all windows 

#ifdef IOBW
	for(int i = 0 ; i < NUM_WIN; i ++)
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
	for(int i = 0; i< NUM_WIN; i++)
	{
		iocompParams->flag[i] = -1; 
	} 

	// Test for window completion 
	for(;;)  
	{
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "ioServer->before MPI Bcast \n"); 
#endif 
		MPI_Bcast(iocompParams->wintestflags, NUM_WIN, MPI_INT, 0, iocompParams->newComm); 
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "ioServer -> after MPI bcast, iocompParams->wintestflags "); 
		for(int i = 0; i < NUM_WIN; i++)
		{
			fprintf(iocompParams->debug, "[%i]", 
					iocompParams->wintestflags[i]); 
		} 
		fprintf(iocompParams->debug, "\n"); 
#endif 

		// iterate across all windows 
		for(int i = 0; i < NUM_WIN; i++)
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
				 * Start window with WIN POST. 
				 * Initialise flag = 0 and start window timer 
				 * Test for window completion 
				 */ 
				winPost(iocompParams, i); 

				winTest(iocompParams, i); 
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
		for(int j = 0; j < NUM_WIN; j++)
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
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServerShared -> loop server exited \n"); 
#endif 
	
	/* 
	 * Clean up after loop server exit, wait for data to be obtained and then free
	 * the window 
	 */ 
	for(int i = 0; i < NUM_WIN; i++)
	{
		// wait for completion of all windows 
		if(iocompParams->flag[i] == 0)
		{
			winWait(iocompParams, i); 
		}
		
		winFree(iocompParams, i); 
	} 

#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServerShared-> all windows freed \n"); 
#endif 
	// Finalise ADIOS2 object
	if(iocompParams->ioLibNum >=2 && iocompParams->ioLibNum <= 4)
	{
		adios2_finalize(iocompParams->adios);
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "ioServer->adios2 finalised\n"); 
#endif	
	} 

#ifdef IOBW
	iobw(iocompParams); 
#endif 

#ifdef VERIFY
	if(!ioRank)
	{
		printf("Verification started \n"); 
	}
	verify(iocompParams); 
#endif

	//#ifndef NODELETE
	//	MPI_Barrier(iocompParams->ioComm); 
	//	if(!ioRank)
	//	{
	//		deleteFiles(iocompParams); 
	//#ifndef NDEBUG 
	//		fprintf(iocompParams->debug, "ioServer->file/directory deleted \n"); 
	//#endif	
	//	} 
	//#endif 
} 

