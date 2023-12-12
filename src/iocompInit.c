/** @file iocompInit.c
 *  @brief Initialises the library. 
 * 	Splits the code into different modes depending on flags.
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h> 
#include "../include/iocomp.h"
#define atoa(x) #x


/**
 * @file iocompInit.c
 * @brief Initialises the library
 * Initialises the library, sets up communicators, and calls ioServerInitialise. 
 * If shared flag is true, then it calls ioServer_shared instead of ioServer.
 * If HT flag is true, then only ioServer is called, and the compute processes 
 * are returned the compServerComm communicator. 
 * Else, ioServerInitialise is called by all processes and function is exited.
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param comm MPI communicator
 * @param FLAG flag for hyperthreading
 * @param ioLibNum number of I/O library to use
 * @param fullNode size of node for communicator splitting
 * @param sharedFlag flag for shared memory usage
 * @param numWin number of windows
 * @return compServerComm communicator for compute processes
 */

/*
 * Initialises the library 
 */
MPI_Comm iocompInit(struct iocomp_params *iocompParams, MPI_Comm comm, bool FLAG, 
		int ioLibNum, int fullNode, bool sharedFlag, int numWin)
{
	int myGlobalrank, ierr;  
	MPI_Comm_rank(comm, &myGlobalrank); 
	iocompParams->globalComm = comm; 
	
#ifdef VERBOSE
	initDebugFile(iocompParams, myGlobalrank);
	fprintf(iocompParams->debug, "MPI initialised \n");
#endif 

	iocompParams->hyperthreadFlag = FLAG; // set hyperthread flag 
	iocompParams->NDIM = NUM_DIM; // set number of dimensions
	iocompParams->ioLibNum = ioLibNum; // set selection of I/O library 
	iocompParams->NODESIZE = fullNode; // set size of node for comm splitting 
	iocompParams->sharedFlag = sharedFlag; // set flag for MPI shared memory usage 
	iocompParams->numWin = numWin; // set the number of windows  
	iocompParams->writeCount = 0; // set counter for number of writes to 0 

	/*
	 * assert tests for input parameters 
	 */ 
	assert(iocompParams->ioLibNum < ioLibCount); 
	assert(iocompParams->ioLibNum >= 0);
	assert(iocompParams->NODESIZE > 0); 

	/* 
	 * Initialise pointers with size of num win 
	 */ 
	iocompParams->wintestflags = (int*)malloc(iocompParams->numWin*sizeof(int)); 
	iocompParams->winMap =(MPI_Win*)malloc(iocompParams->numWin*sizeof(MPI_Win)); 
	iocompParams->flag = (int*)malloc(iocompParams->numWin*sizeof(int)); 
	iocompParams->array = (double**)malloc(iocompParams->numWin*sizeof(double*)); 
	iocompParams->writeFile = (char**)malloc(iocompParams->numWin*sizeof(char*)); 
	/*
	 * array parameters using ndim 
	 */ 
	iocompParams->arrayStart = malloc(sizeof(size_t)*iocompParams->NDIM);
	iocompParams->localArray = malloc(sizeof(size_t)*iocompParams->NDIM);
	iocompParams->globalArray = malloc(sizeof(size_t)*iocompParams->NDIM);
	

	/*
	 * If the shared flag is on and process is ioserver then ioServer initialises
	 * shared windows. 
	 */ 
	if(sharedFlag == true)
	{

		iocompParams->sharedFlag = true; 

		// split communicators 
		commSplit_shared(iocompParams);  

		// find new rank for the pair communicator between IO and Compute rank 
		int newRank; 
		ierr = MPI_Comm_rank(iocompParams->newComm,&newRank); 
		mpi_error_check(ierr);

		if(newRank != 0)
		{
#ifdef VERBOSE
			fprintf(iocompParams->debug,"iocompInit -> Before assigining groups for io server \n"); 
#endif
			// allocate groups 
			/*
			 * groups newComm communicator's rank 0 and 1 into a group
			 * comp process initialises array and creates a window with that array
			 */
			MPI_Group comm_group;
			int ranks[2];
			for (int i=0;i<2;i++) {
				ranks[i] = i;     //For forming groups, later
			}
			MPI_Comm_group(iocompParams->newComm,&comm_group);

			/* I/O group consists of ranks 1 */
			MPI_Group_incl(comm_group,1,ranks,&iocompParams->group);
#ifdef VERBOSE
			fprintf(iocompParams->debug,"iocompInit -> After assigning groups for io server\n"); 
#endif

			ioServer_shared(iocompParams);
#ifdef VERBOSE
			fprintf(iocompParams->debug,"iocompInit -> After ioServer shared exits \n"); 
#endif

			/* write timers to file after reducing across ranks*/
#ifndef NOTIMERS
			printWriteTimers(iocompParams); 
#endif

			MPI_Finalize(); 
#ifdef VERBOSE 
			fprintf(iocompParams->debug, "iocompInit-> after MPI finalize \n"); 
#endif 
		  free(iocompParams->writeFile); 
		  iocompParams->writeFile = NULL; 
			exit(0); 
		}
		else
		{
			/*
			 * groups newComm communicator's rank 0 and 1 into a group
			 * comp process initialises array and creates a window with that array
			 */
			MPI_Group comm_group;
			int ranks[2];
			for (int i=0;i<2;i++) {
				ranks[i] = i;     //For forming groups, later
			}
			MPI_Comm_group(iocompParams->newComm,&comm_group);

			/* I/O group consists of ranks 1 */
			MPI_Group_incl(comm_group,1,ranks+1,&iocompParams->group);
#ifdef VERBOSE
			fprintf(iocompParams->debug,"iocompInit -> After assigning groups for comp server\n"); 
#endif
		} 
	} 
	else
	{
		/*
		 * comm split splits communicators in 2, assigns colour to ranks
		 * assigns communicators in struct for both cases  
		 */ 
		comm_split(iocompParams, comm); 
#ifdef VERBOSE
		fprintf(iocompParams->debug, "iocompInit -> communicator split up and colour assigned \n"); 
#endif
		/*
		 * If HT flag is on, then called by io server, if HT flag off then called by
		 * every process. This is because only ioServers have access to ioServerComm.
		 * ioServerInitialise initialises cartesian communicator and adios2 objects,
		 * and sets up other I/O related variables.
		 */ 
		if(!iocompParams->hyperthreadFlag || iocompParams->colour==ioColour) 
		{
			ioServerInitialise(iocompParams); 
		}


		/*
		 * If HT flag is on and process is ioserver then ioServer recieves data
		 * and finalises adios2 object & mpi finalizes and program exits. 
		 */ 
		if(iocompParams->hyperthreadFlag && iocompParams->colour == ioColour)
		{
			int ioRank; 
			MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
#ifdef VERBOSE
			fprintf(iocompParams->debug,"ioServerInitialise -> ioServer called\n"); 
#endif
			ioServer(iocompParams);
#ifdef VERBOSE
			fprintf(iocompParams->debug,"ioServerInitialise -> After ioServer\n"); 
#endif

			/* write timers to file after reducing across ranks*/
#ifndef NOTIMERS
			printWriteTimers(iocompParams); 
#endif

			MPI_Finalize(); 
#ifdef VERBOSE
			fprintf(iocompParams->debug,"ioServerInitialise -> After finalize\n"); 
#endif
			exit(0); 
		}

	} 

	/*
	 * Return comp server comm to compute processes if HT flag is on
	 * but if HT flag is off, and there is no splitting, then the world comm 
	 * is returned
	 */ 
	return(iocompParams->compServerComm); 
} 

