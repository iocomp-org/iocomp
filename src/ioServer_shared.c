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

	// allocate windows 
	double* array[NUM_WIN]; 
	MPI_Win win_ptr[NUM_WIN]; 
	int soi = sizeof(double); 

	// allocate shared windows 
	for(int i = 0; i < NUM_WIN; i++)
	{
		int ierr = MPI_Win_allocate_shared(0, soi, MPI_INFO_NULL, iocompParams->newComm, &array[i], &win_ptr[i]); 
		mpi_error_check(ierr);
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "ioServer -> MPI allocated window %i \n", i); 
#endif 
	}

	// allocate arrays using window pointers 
	for(int i = 0; i < NUM_WIN; i++)
	{
		long int arraySize; 
		int dispUnit; 
		int ierr; 
		ierr = MPI_Win_shared_query(win_ptr[i], 0, &arraySize, &dispUnit, &array[i]); 
		mpi_error_check(ierr); 
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "ioServer -> MPI shared query %i \n", i); 
#endif 
	} 

	// initialise IO Params structure 
	iocompParams->ioComm = iocompParams->ioServerComm; 
	int ioRank, ierr; 
	ierr = MPI_Comm_rank(iocompParams->ioComm, &ioRank); 
	mpi_error_check(ierr); 

	// Allocate cartesian communicator, adios2 objects	
	ioServerInitialise(iocompParams); 

	// Initialise array parameters for each process write into a global file  
	// Local data size needs to be obtained somehow. 
	// Setting it to 100 for now.
	iocompParams->localDataSize = 100; 
	arrayParamsInit(iocompParams, iocompParams->ioServerComm); 

	for(int i = 0; i < NUM_WIN; i++)
	{
		iocompParams->wintestflags[i] = 0; 
	} 
	// declare mult variable to test for completion among all windows 
	int wintestmult = 1; 
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "ioServer -> after win test flags initialised\n"); 
#endif 

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
					// wait for window completion 
					ierr = MPI_Win_wait(win_ptr[i]); 
					mpi_error_check(ierr); 
#ifndef NDEBUG 
					fprintf(iocompParams->debug, "ioServer window:%i flag negative and win wait implemented\n", i); 
#endif 
					ioLibraries(array[i], iocompParams); 
					flag[i] = 1; 
				}

				ierr = MPI_Win_post(iocompParams->group, 0, win_ptr[i]);
				mpi_error_check(ierr); 
#ifndef NDEBUG 
					fprintf(iocompParams->debug, "ioServerShared->window %i after win post\n", i); 
#endif 
				flag[i] = 0; // window activated 
#ifdef IOBW	
				iocompParams->winTime_start[i] = MPI_Wtime();
#endif 

				// test for window completion 	
				ierr = MPI_Win_test(win_ptr[i], &flag[i]); 
				mpi_error_check(ierr);
#ifndef NDEBUG 
				fprintf(iocompParams->debug, "ioServerShared->window:%i after win test\n",i); 
#endif 
				// if window is available to print then print and end timer 
				if(flag[i])
				{
#ifndef NDEBUG 
					fprintf(iocompParams->debug, "ioServerShared->window:%i flag positive \n",i); 
#endif
					ioLibraries(array[i], iocompParams); 
				}
			} 
			else if(iocompParams->wintestflags[i] == WIN_TEST && flag[i] == 0)
			{
				// test for window completion only if previous tests have come back
				// unsuccessfully
				ierr = MPI_Win_test(win_ptr[i], &flag[i]); 
				mpi_error_check(ierr);
#ifndef NDEBUG 
				fprintf(iocompParams->debug, "ioServerShared->window:%i after win test\n",i); 
#endif 
				// if window is available to print then print and end timer 
				if(flag[i])
				{
#ifndef NDEBUG 
					fprintf(iocompParams->debug, "ioServerShared->window:%i flag positive \n",i); 
#endif
					ioLibraries(array[i], iocompParams); 

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
			ierr = MPI_Win_wait(win_ptr[i]); 
			mpi_error_check(ierr); 
#ifndef NDEBUG 
			fprintf(iocompParams->debug, "ioServerShared-> window:%i ioLibraries called after win wait\n",i); 
#endif 
			ioLibraries(array[i], iocompParams); 
		}
		
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "ioServerShared-> window:%i before win free reached\n",i); 
#endif 
		ierr = MPI_Win_free(&win_ptr[i]);
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

