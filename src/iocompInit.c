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

/*
 * Initialises the library 
 */
MPI_Comm iocompInit(struct iocomp_params *iocompParams, MPI_Comm comm, bool FLAG, 
		int ioLibNum, int fullNode, bool sharedFlag, int numWin)
{
	int myGlobalrank, ierr;  
	MPI_Comm_rank(comm, &myGlobalrank); 
	iocompParams->globalComm = comm; 

#ifndef NDEBUG
	initDebugFile(iocompParams, myGlobalrank);
	fprintf(iocompParams->debug, "MPI initialised \n");
#endif 

	iocompParams->hyperthreadFlag = FLAG; // set hyperthread flag 
	iocompParams->NDIM = NUM_DIM; // set number of dimensions
	iocompParams->ioLibNum = ioLibNum; // set selection of I/O library 
	iocompParams->NODESIZE = fullNode; // set size of node for comm splitting 
	iocompParams->sharedFlag = sharedFlag; // set flag for MPI shared memory usage 
	iocompParams->numWin = numWin; // set the number of windows  

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
	iocompParams->winMap =(int*)malloc(iocompParams->numWin*sizeof(int)); 
	iocompParams->flag = (int*)malloc(iocompParams->numWin*sizeof(int)); 
	iocompParams->array = (double**)malloc(iocompParams->numWin*sizeof(double*)); 
	iocompParams->writeFile = (char**)malloc(iocompParams->numWin*sizeof(char*)); 

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
#ifndef NDEBUG
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
#ifndef NDEBUG
			fprintf(iocompParams->debug,"iocompInit -> After assigning groups for io server\n"); 
#endif

			ioServer_shared(iocompParams);
#ifndef NDEBUG
			fprintf(iocompParams->debug,"iocompInit -> After ioServer shared exits \n"); 
#endif
			MPI_Finalize(); 
#ifndef NDEBUG 
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
#ifndef NDEBUG
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
#ifndef NDEBUG
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
#ifndef NDEBUG
			fprintf(iocompParams->debug,"ioServerInitialise -> ioServer called\n"); 
#endif
			ioServer(iocompParams);
#ifndef NDEBUG
			fprintf(iocompParams->debug,"ioServerInitialise -> After ioServer\n"); 
#endif
#ifdef TESTING
			// all files have been written, testing can commence. 
			printf("IO server barrier reached\n"); 
		  MPI_Barrier(comm);  
#endif 
			printf("before finalize \n"); 
			MPI_Finalize(); 
#ifndef NDEBUG
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

