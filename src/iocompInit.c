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
		int ioLibNum, int fullNode, bool sharedFlag)
{
	int myGlobalrank; 
	MPI_Comm_rank(comm, &myGlobalrank); 
#ifndef NDEBUG
	char rank_str[10]; 
	sprintf(rank_str, "%d", myGlobalrank);

	strcat(iocompParams->DEBUG_FILE,rank_str ); 
	strcat(iocompParams->DEBUG_FILE,".out"); 
	printf("debug file name %s\n",iocompParams->DEBUG_FILE); 

	iocompParams->debug_out=fopen(iocompParams->DEBUG_FILE,"w+"); // create new file 
	if(iocompParams->debug_out==NULL)
	{
		printf("debug file not created \n"); 
		exit(1); 
	}
	// fprintf(iocompParams->debug_out, "iocompInit \n"); 
	// VERBOSE_1(myGlobalrank, "iocompInit -> Start of intercomm_init\n"); 
	VERBOSE_2(iocompParams->debug_out, "iocompInit -> Start of intercomm_init\n"); 
#endif

	iocompParams->hyperthreadFlag = FLAG; // set hyperthread flag 
	iocompParams->NDIM = NUM_DIM; // set number of dimensions
	iocompParams->ioLibNum = ioLibNum; // set selection of I/O library 
	iocompParams->NODESIZE = fullNode; // set size of node for comm splitting 

	/*
	 * assert tests for input parameters 
	 */ 
	assert(iocompParams->ioLibNum < ioLibCount); 
	assert(iocompParams->ioLibNum >= 0);
	assert(iocompParams->NODESIZE > 0); 

#ifndef NDEBUG
	VERBOSE_1(myGlobalrank,"iocompInit -> variables declared flag %i, ndim %i, iolib %i\n", 
			iocompParams->hyperthreadFlag, iocompParams->NDIM, iocompParams->ioLibNum); 
	VERBOSE_2(iocompParams->debug_out,
			"iocompInit -> variables declared flag %i, ndim %i, iolib %i\n", 
			iocompParams->hyperthreadFlag, iocompParams->NDIM, iocompParams->ioLibNum); 
#endif
	/*
	 * comm split splits communicators in 2, assigns colour to ranks
	 * assigns communicators in struct for both cases  
	 */ 
	comm_split(iocompParams, comm); 
#ifndef NDEBUG
	VERBOSE_1(myGlobalrank, "iocompInit -> communicator split up and colour assigned \n"); 
	VERBOSE_2(iocompParams->debug_out, "iocompInit -> communicator split up and colour assigned \n"); 
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
		VERBOSE_1(ioRank,"ioServerInitialise -> ioServer called\n"); 
		VERBOSE_2(iocompParams->debug_out,"ioServerInitialise -> ioServer called\n"); 
#endif
		ioServer(iocompParams);
#ifndef NDEBUG
		VERBOSE_1(ioRank,"ioServerInitialise -> After ioServer\n"); 
		VERBOSE_2(iocompParams->debug_out,"ioServerInitialise -> After ioServer\n"); 
#endif
		MPI_Finalize(); 
#ifndef NDEBUG
		VERBOSE_1(ioRank,"ioServerInitialise -> After finalize\n"); 
		VERBOSE_2(iocompParams->debug_out,"ioServerInitialise -> After finalize\n"); 
#endif
		exit(0); 
	}

	/*
	 * If the shared flag is on and process is ioserver then ioServer initialises
	 * shared windows. 
	 */ 
	if( (sharedFlag == true) && (iocompParams->colour == ioColour) )
	{
		iocompParams->sharedFlag = true; 
		ioServer_shared(iocompParams);
		MPI_Finalize(); 
	}

	/*
	 * Return comp server comm to compute processes if HT flag is on
	 * but if HT flag is off, and there is no splitting, then the world comm 
	 * is returned
	 */ 
	return(iocompParams->compServerComm); 
} 

