#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"
#define filename "write_time.csv"
void ioServer(struct iocomp_params *iocompParams)
{

#ifndef NDEBUG
	printf("ioServer -> started\n"); 
#endif
	int i, ierr; 
	int ioRank; 
	double timerStart, timerEnd; 
	timerStart = timerEnd = 0.0; 

	ierr = MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
	mpi_error_check(ierr); 

	MPI_Status status;
	MPI_Request request; 
	MPI_Info info;  

	/*
	 * Recieving data starts using interComm
	 * source is ioRank in interComm, as that is the 
	 * rank of the paired ComputeProcess in computeComm 
	 */ 

	int source, tag; 

	source = getPair(iocompParams); 
	tag = source; 

#ifndef NDEBUG
	printf("ioServer -> Recieving data starts from source rank %i \n", source); 
#endif
	int test_probe; 
	int test_count = 1; 
	int iter = 0; 

	/*
	 * Check for ghost messages, for ever loop activated
	 */ 
	for(;;) 
	{
#ifndef NDEBUG
		printf("ioServer -> start of ioServer loop, iter %i ioRank %i \n", iter, ioRank ); 
#endif
		MPI_Probe(source, tag, iocompParams->globalComm, &status); // Probe for additional messages. 
		MPI_Get_count(&status, MPI_DOUBLE, &test_count); // get count 

		/*
		 * Check for count = 0, means stop recieving messages 
		 */ 
		if(!test_count)
		{
#ifndef NDEBUG
			printf("ioServer -> ghost messaged recieved \n"); 	
#endif
			int ghost;  
			ierr = MPI_Recv(&ghost, 0, MPI_INT, source, tag,
					iocompParams->globalComm,&status);
			mpi_error_check(ierr); 
			break; 
		}

		/*
		 * Else recieve message 
		 */ 
		else if(test_cout>0)
		{
			// initialise recv array 
			double* recv = NULL; 
			iocompParams->localDataSize = test_count; 
			assert(iocompParams->localDataSize>0); // check for negative values 
			recv = (double*)malloc(iocompParams->localDataSize*sizeof(double)); // one rank only sends to one rank
			malloc_check(recv); 

#ifndef NDEBUG
			printf("ioServer -> Initialisation of recv array with count %i \n", iocompParams->localDataSize); 
#endif
			ierr = MPI_Recv(recv, test_count, MPI_DOUBLE, source, tag,
					iocompParams->globalComm,&status);
			mpi_error_check(ierr); 

#ifndef NDEBUG
			printf("ioServer -> Recv data coming from rank %i \n",source ); 
			//for(i = 0; i < (int)iocompParams->localDataSize; i++) { printf("%lf ",recv[i]); }// init size in each dimension to be n. For ex. NDIM = 2 will reslt in n x n }
			printf("\n"); 
#endif

			/*
			 * Writing data using different IO libraries commences using io_libraries function
			 * Parameters passed using iocompParams  
			 */ 

#ifndef NDEBUG
			printf("ioServer -> Send to ioLibraries \n"); 
#endif
			ioLibraries(recv, iocompParams); 
			free(recv);
			recv = NULL; 
	}  
	iter++; 
} 
} 
