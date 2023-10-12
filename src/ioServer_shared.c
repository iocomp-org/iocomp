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

	// allocate shared windows and their pointers 
	ioServer_sharedAllocate(iocompParams); 

	// Allocate cartesian communicator, adios2 objects	
	ioServerInitialise(iocompParams); 

	arrayParamsInit(iocompParams, iocompParams->ioServerComm); 

	// declare mult variable to test for completion among all windows 
	int wintestmult = 1; 

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
	int flag[NUM_WIN]; 	
	for(int i = 0; i< NUM_WIN; i++)
	{
		flag[i] = -1; 
	} 

	// Test for window completion 
	do 
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
				 * in this case WIN WAIT is coming before WIN POST, but it assumes that 
				 * WIN POST has been called before.
				 * test for flag = 0 checks if window has been written before to avoid
				 * overwriting, IF win_test completes the window
				 */ 
				if(iocompParams->wintestflags[i]==WIN_WAIT && flag[i]==0)  
				{
#ifndef NDEBUG 
					fprintf(iocompParams->debug, "ioServer window:%i flag value:%i before win wait implemented\n", i, flag[i]); 
#endif 
					// wait for window completion 
					ierr = MPI_Win_wait(iocompParams->winMap[i]); 
					mpi_error_check(ierr); 
#ifndef NDEBUG 
					fprintf(iocompParams->debug, "ioServer window:%i flag value:%i after  win wait implemented\n", i, flag[i]); 
#endif 
					ioLibraries(iocompParams->array[i], iocompParams); 
					flag[i] = 1; 
				}

				ierr = MPI_Win_post(iocompParams->group, 0, iocompParams->winMap[i]);
				mpi_error_check(ierr); 
#ifndef NDEBUG 
				fprintf(iocompParams->debug, "ioServerShared->window %i after win post\n", i); 
#endif 
				flag[i] = 0; // window activated 
#ifdef IOBW	
				iocompParams->winTime_start[i] = MPI_Wtime();
#endif 

				// test for window completion 	
				ierr = MPI_Win_test(iocompParams->winMap[i], &flag[i]); 
				mpi_error_check(ierr);
#ifndef NDEBUG 
				fprintf(iocompParams->debug, "ioServerShared->window:%i after win test with flag:%i\n",i, flag[i]); 
#endif 
				// if window is available to print then print and end timer 
				if(flag[i])
				{
#ifndef NDEBUG 
					fprintf(iocompParams->debug, "ioServerShared->window:%i flag positive value:%i \n",i, flag[i]); 
#endif
					ioLibraries(iocompParams->array[i], iocompParams); 
				}
			} 
			else if(iocompParams->wintestflags[i] == WIN_TEST && flag[i] == 0)
			{
				// test for window completion only if previous tests have come back
				// unsuccessfully
				ierr = MPI_Win_test(iocompParams->winMap[i], &flag[i]); 
				mpi_error_check(ierr);
#ifndef NDEBUG 
				fprintf(iocompParams->debug, "ioServerShared->window:%i after win test flag value:%i\n",i, flag[i]); 
#endif 
				// if window is available to print then print and end timer 
				if(flag[i])
				{
#ifndef NDEBUG 
					fprintf(iocompParams->debug, "ioServerShared->window:%i flag value:%i \n",i, flag[i]); 
#endif
					ioLibraries(iocompParams->array[i], iocompParams); 
					flag[i] = 1; 

				}
			} 
		}

		// check if no more messages left 
		wintestmult = 0;  // reset value 
		for(int j = 0; j < NUM_WIN; j++)
		{
			if(iocompParams->wintestflags[j] == WIN_FREE)
			{
				wintestmult = 1; 
			} 
		} 
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "ioServerShared -> wintestmult value %i\n", wintestmult); 
#endif 
	}while(!wintestmult);  // test for completion of all windows 

#ifndef NDEBUG 
	fprintf(iocompParams->debug, "ioServerShared -> loop server exited \n"); 
#endif 

	// free windows and pointer 
	// while freeing, check if there are any opened windows 
	for(int i = 0; i < NUM_WIN; i++)
	{
		// wait for completion of all windows 
		if(flag[i] == 0)
		{
#ifndef NDEBUG 
			fprintf(iocompParams->debug, "ioServerShared->window:%i Before win wait \n",i); 
#endif 
			ierr = MPI_Win_wait(iocompParams->winMap[i]); 
			mpi_error_check(ierr); 
#ifndef NDEBUG 
			fprintf(iocompParams->debug, "ioServerShared-> window:%i After win wait\n",i); 
#endif 
			ioLibraries(iocompParams->array[i], iocompParams); 
			flag[i] = 1; 
		}

#ifndef NDEBUG 
		fprintf(iocompParams->debug, "ioServerShared-> window:%i before win free reached\n",i); 
#endif 
		ierr = MPI_Win_free(&iocompParams->winMap[i]);
		mpi_error_check(ierr); 
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "ioServerShared-> window:%i after win free \n",i); 
#endif 
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

