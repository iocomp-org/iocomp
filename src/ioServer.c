/** @file ioServer.c 
 *  @brief Contains functions for I/O server for HT flag and sequential mode. 
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
#define filename "write_time.csv"

/**
 * @file ioServer.c
 * @brief Contains functions for I/O server for HT flag. 
 * For HT mode, initialises the server for loop which gets the length of the message 
 * using MPI_Probe. 
 * If message length is 0 i.e. a ghost message it exits the loop. 
 * Otherwise, it receives the message and writes it using the appropriate I/O library.
 * If message length is same as previous length, it keeps the previous initialisations. 
 * After receiving the message, it recieves the file name and writes it under that file name.
 * 
 * @param iocompParams pointer to struct containing all parameters for the library
 * @return void 
 */

void ioServer(struct iocomp_params *iocompParams)
{

	int ierr, ioRank; 

	ierr = MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
	mpi_error_check(ierr); 

	MPI_Status status;

	/*
	 * Recieving data starts using interComm
	 * source is ioRank in interComm, as that is the 
	 * rank of the paired ComputeProcess in computeComm 
	 */ 
	int source, tag; 
	source = getPair(iocompParams); 
	tag = source; 

#ifdef VERBOSE
	fprintf(iocompParams->debug,"ioServer -> Recieving data starts from source rank %i \n", source); 
#endif
	int test_count = 1; 
	int iter = 0; 

	/*
	 * initialise recv array 
	 * check previousCount to prevent repeated mallocs 
	 * set to -1 meaning no prior mallocing 
	 */ 
	double* recv = NULL; 
	int previousCount = -1; 
	/*
	 * Check for ghost messages, for ever loop activated
	 */ 
	for(;;) 
	{
#ifdef VERBOSE
		// fprintf(iocompParams->debug,"ioServer -> start of ioServer loop, iter %i ioRank %i \n",iter,ioRank); 
#endif

		MPI_Probe(source, tag, iocompParams->globalComm, &status); // Probe for additional messages
#ifdef VERBOSE
		// fprintf(iocompParams->debug,"ioServer -> MPI probe called \n"); 
#endif

		MPI_Get_count(&status, MPI_DOUBLE, &test_count); // get count 
#ifdef VERBOSE
		// fprintf(iocompParams->debug,"ioServer -> MPI get count %i \n", test_count); 
#endif

		/*
		 * Check for count = 0, means stop recieving messages 
		 */ 
		if(!test_count)
		{
#ifdef VERBOSE
			fprintf(iocompParams->debug,"ioServer -> test count = 0, ghost message initialised \n"); 	
#endif
			int ghost;  
			ierr = MPI_Recv(&ghost, 0, MPI_INT, source, tag,
					iocompParams->globalComm,&status);
			mpi_error_check(ierr); 
#ifdef VERBOSE
			fprintf(iocompParams->debug,"ioServer -> ghost message recieved \n"); 	
#endif
			/*
			 * Stop send reached, so recv array will be freed
			 * and adios2 object is finalised. 
			 */ 
			free(recv);
			recv = NULL; 
#ifdef VERBOSE
			fprintf(iocompParams->debug,"ioServer -> recv array freed\n"); 	
#endif
#ifndef NOADIOS2
			if(iocompParams->ioLibNum >=2 && iocompParams->ioLibNum <= 4)
			{
				adios2_finalize(iocompParams->adios); 
			} 
#endif 
			break; 
		}

		/*
		 * Else recieve message 
		 */ 
		else if(test_count>0)
		{
			/*
			 * if the message length is different from last sending OR 
			 * this is the first time message is sent.
			 * in first case free the recv array and reinitialise with new
			 * dimensions.
			 * in second case, only initialise with new dimensions
			 */ 
			if(test_count != previousCount) 
			{
				iocompParams->localDataSize = test_count; 
				assert(iocompParams->localDataSize>0); // check for negative values 

				/*
				 * if recv has been allocated previously 
				 * clear memory then reallocate 
				 */
				if(iocompParams->previousInit) 
				{
					free(recv);
					recv = NULL; 
				} 
				/*
				 * sets local size, global size, array offsets for ioLibraries
				 * only IF message recvd, and has not been initialised with the same
				 * count as before. 
				 */ 
				else
				{
					iocompParams->previousInit = 1; 
					arrayParamsInit(iocompParams,iocompParams->ioServerComm); 
				}	
				recv = (double*)malloc(iocompParams->localDataSize*sizeof(double)); // one rank only sends to one rank
				malloc_check(recv); 
				previousCount = test_count;  
			} 
#ifdef VERBOSE
			fprintf(iocompParams->debug,"ioServer -> Initialisation of recv array with count %li \n", iocompParams->localDataSize); 
#endif
			ierr = MPI_Recv(recv, test_count, MPI_DOUBLE, source, tag,
					iocompParams->globalComm,&status);
			mpi_error_check(ierr); 
#ifdef VERBOSE
			fprintf(iocompParams->debug,"ioServer -> Received array \n"); 
#endif
			// get file name 
			getFileName(iocompParams,0);  

#ifdef VERBOSE
			fprintf(iocompParams->debug,"ioServer -> Recv data coming from rank %i \n",source ); 
#endif

			/*
			 * Writing data using different IO libraries commences using io_libraries function
			 * Parameters passed using iocompParams  
			 */ 
#ifdef VERBOSE
			fprintf(iocompParams->debug,"ioServer -> Send to ioLibraries \n"); 
#endif
			ioLibraries(recv, iocompParams, 0); 
		}  

		iter++; 
	} 
} 
