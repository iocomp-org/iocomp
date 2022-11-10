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
	printf("ioServer started\n"); 
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

	// initialise recv array 
	double* recv = NULL; 
	recv = (double*)malloc(iocompParams->localDataSize*sizeof(double)); // one rank only sends to one rank
	malloc_check(recv); 

	for ( i = 0; i < iocompParams->localDataSize; i++)
	{
		recv[i] = 0; // initialise recv array 
	}
#ifndef NDEBUG
	printf("Initialisation of recv array \n"); 
#endif

	/*
	 * Recieving data starts using interComm
	 * source is ioRank in interComm, as that is the 
	 * rank of the paired ComputeProcess in computeComm 
	 */ 

	int source, tag; 

	source = getPair(iocompParams); 
	tag = source; 

#ifndef NDEBUG
	printf("Recieving data starts from source rank %i \n", source); 
#endif
	int test_probe, test_count; 
	test_count = 1; 
	
	/*
	 * Check for ghost messages, for ever loop activated
	 */ 
	for(;;) 
	{
		MPI_Probe(source, tag, iocompParams->globalComm, &status); // Probe for additional messages. 
		MPI_Get_count(&status, MPI_DOUBLE, &test_count); // get count 

		printf("value of mpi count %i \n",test_count); 
		
		if(!test_count)
		{
			printf("ghost messaged recieved \n"); 	
			break; 
		}

		else if(test_count!= iocompParams->localDataSize)
		{
			printf("complete message not recieved \n"); 
			exit(0); 
		}
		ierr = MPI_Recv(recv, iocompParams->localDataSize, iocompParams->dataType, source, tag,
				iocompParams->globalComm,&status);

		// ierr = MPI_Irecv(recv, iocompParams->localDataSize, iocompParams->dataType, source, tag,
		// 		iocompParams->interComm, &request); 
		mpi_error_check(ierr); 

		// ierr = 	MPI_Waitall(1, &request, &status); // wait for all processes to finish sending and recieving  
		// mpi_error_check(ierr); 

#ifndef NDEBUG
		printf("Recv data coming from rank %i \n",source ); 
		for(i = 0; i < iocompParams->localDataSize; i++)
		{
			printf("%lf ",recv[i]); // init size in each dimension to be n. For ex. NDIM = 2 will reslt in n x n 
		}
		printf("\n"); 
#endif

		/*
		 * Writing data using different IO libraries commences using io_libraries function
		 * Parameters passed using iocompParams  
		 */ 

		ioLibraries(recv, iocompParams); 
	}  


	free(recv);
	recv = NULL; 

} 
