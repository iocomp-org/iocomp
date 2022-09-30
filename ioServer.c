#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"
#define filename "write_time.csv"
void ioServer(struct iocomp_params *iocompParams)
{

#ifndef NDEBUG
	printf("ioServer started\n"); 
#endif
	int i, ierr, globalRank, globalSize; 
	int ioRank, ioSize; 
	double timerStart, timerEnd; 
	timerStart = timerEnd = 0.0; 

	ierr = MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_size(iocompParams->ioServerComm, &ioSize); 
	mpi_error_check(ierr); 

	MPI_Status status;
	MPI_Request request; 
	MPI_Info info;  

	// initialise recv array 
	double* recv = NULL; 
	recv = (double*)malloc(iocompParams->globalDataSize*sizeof(double)); // one rank only sends to one rank
	for ( i = 0; i < iocompParams->globalDataSize; i++)
	{
		recv[i] = 0; // initialise recv array 
	}
#ifndef NDEBUG
	printf("Initialisation of recv array \n"); 
#endif

	/*
	 * Assign arraystart position for writing of array
	 * Assuming weak scaling. Outerdimension would have n*totalrank
	 */

#ifndef NDEBUG
	printf("Recieving data starts \n"); 
#endif

	/*
	 * Recieving data starts using interComm
	 * source is ioRank in interComm, as that is the 
	 * rank of the paired ComputeProcess in computeComm 
	 */ 

	int source, tag; 
	source = ioRank; 
	tag = ioRank; 
	ierr = MPI_Irecv(recv, iocompParams->localDataSize, MPI_DOUBLE, source, tag,
			iocompParams->interComm, &request); 
	mpi_error_check(ierr); 

	ierr = 	MPI_Waitall(1, &request, &status); // wait for all processes to finish sending and recieving  
	mpi_error_check(ierr); 

	// print out recieved data 
#ifndef NDEBUG
	printf("Recv data coming from rank %i \n",source ); 
	for(i = 0; i < iocompParams->localDataSize; i++)
	{
		printf("%lf ",recv[i]); // init size in each dimension to be n. For ex. NDIM = 2 will reslt in n x n 
	}
	printf("\n"); 
#endif

	if (ioRank == 0) // timing will be measured by using ioRank = 0 
	{	
		timerStart = MPI_Wtime();
	}

	/*
	 * Writing data using different IO libraries commences using io_libraries function
	 * Parameters passed using iocompParams  
	 */ 

	ioLibraries(recv, iocompParams); 
	MPI_Barrier(iocompParams->ioServerComm); // Wait for all processes to finish  

#ifndef NDEBUG
	printf("MPI barrier \n"); 
#endif

	if (ioRank == 0)
	{
		timerEnd  = MPI_Wtime();
		iocompParams->writeTime = timerEnd - timerStart; 
	} 

	free(recv);
	recv = NULL; 

	if (ioRank == 0) 
	{
		data_output(iocompParams); 
	}
} 
