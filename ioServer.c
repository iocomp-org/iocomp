#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"
#define filename "write_time.csv"
void ioServer(struct iocomp_params *iocompParams)
{

	printf("ioServer started\n"); 
#ifndef NDEBUG
	printf("ioServer started\n"); 
#endif
	int i, ierr, globalRank, globalSize; 
	int ioRank, ioSize; 
	double timerStart, timerEnd; 
	timerStart = 0; 
	timerEnd = 0; 

	ierr = MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_size(iocompParams->ioServerComm, &ioSize); 
	mpi_error_check(ierr); 

	MPI_Status status;
	MPI_Request request; 
	MPI_Info info;  

	printf("MPI stuff \n"); 
	/*
	 * Initialisation of localsize, global size, localdatasize and globaldatasize
	 */ 
	/*
	for (i = 0; i< iocompParams->NDIM; i++)
	{
		iocompParams->globalSize[i] = iocompParams->localSize[i]; 
		iocompParams->localDataSize *= iocompParams->localSize[i]; 
	}

	iocompParams->globalSize[0] = iocompParams->localSize[0]*iocompParams->compServerSize; 

	for (i = 0; i< iocompParams->NDIM; i++)
	{
		iocompParams->globalDataSize *= iocompParams->globalSize[i]; 
	}
*/ 


	printf("globaldatasize %i localdatasize %i \n", iocompParams->globalDataSize, iocompParams->localDataSize); 
#ifndef NDEBUG
	printf("globaldatasize %i localdatasize %i \n", iocompParams->globalDataSize, iocompParams->localDataSize); 
#endif

	// initialise recv array 
	double* recv = NULL; 
	recv = (double*)malloc(iocompParams->globalDataSize*sizeof(double)); // one rank only sends to one rank

	printf("Declaration of recv array \n"); 
#ifndef NDEBUG
	printf("Initialisation of recv array \n"); 
#endif

	for ( i = 0; i < iocompParams->globalDataSize; i++)
	{
		recv[i] = 0; // initialise recv array 
	}

	printf("Initialisation of recv array \n"); 
	/*
	 * Go to each compute rank and get data from it 
	 * Assuming compute ranks are total ranks - 1 
	 */ 

	int computeRank = 0; 

	/*
	 * Assign arraystart position for writing of array
	 * Assuming weak scaling. Outerdimension would have n*totalrank
	 */

	int arrayStart_local[iocompParams->NDIM]; 	
	iocompParams->arrayStart = arrayStart_local; 
	
	for(int i = 0; i < iocompParams->NDIM; i++)
	{
		iocompParams->arrayStart[i] = 0; 
	}
	iocompParams->arrayStart[0] = ioRank * iocompParams->localSize[0]; // assuming ar_size has uniform dimensions. 

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
	printf("Recieving data starts \n"); 
	//ierr = MPI_Irecv(recv, iocompParams->localDataSize, MPI_DOUBLE, source, tag,
	//		iocompParams->interComm, &request); 
	mpi_error_check(ierr); 
	printf("Irecv completed \n");

	ierr = 	MPI_Waitall(1, &request, &status); // wait for all processes to finish sending and recieving  
	mpi_error_check(ierr); 

#ifndef NDEBUG
	// print out recieved data 
	printf("Recv data coming from rank %i \n",computeRank); 
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

	printf("timer start\n"); 
	/*
	 * Writing data using different IO libraries commences using io_libraries function
	 * Parameters passed using iocompParams  
	 */ 

	//ioLibraries(recv, iocompParams); 
	//MPI_Barrier(iocompParams->ioServerComm); // Wait for all processes to finish  

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

/*	if (ioRank == 0) 
	{
		data_output(iocompParams); 
	}
*/ 

	printf("ioServer exiting \n");
} 
