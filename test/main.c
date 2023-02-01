#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "test.h"
#define LOOP_COUNT 10

int main(int argc, char** argv)
{

  struct iocomp_params iocompParams; // struct for iocomp library 
	struct test_variables testParams; // struct for test program
  
	int ierr;
  ierr = MPI_Init(&argc, &argv);  
  mpi_error_check(ierr); 

  MPI_Comm comm; 
  MPI_Comm_dup(MPI_COMM_WORLD,&comm); 
  int rank, size; 
  MPI_Comm_rank(comm, &rank); 
  MPI_Comm_size(comm, &size); 
	testParams.myrank = rank; 
	testParams.mysize = size; 

	/*
	 * initialise test function and populate iocompParams, returning data array to write. 
	 */ 
	double* data = initialise(&iocompParams,&testParams,comm); 

	ioServerInitialise(&iocompParams, testParams.ioLibNum); 
	MPI_Request request; 
	
	/* 
	 * Send data multiple times, output timings and other data to a file 
	 */
	for(int i = 0; i < LOOP_COUNT; i++)
	{
		testParams.startTime[i] = MPI_Wtime();  
		dataSend(data,&iocompParams, &request); // send data off using dataSend
		dataWait(&iocompParams,&request);
		stopSend(&iocompParams); 
		testParams.endTime[i] = MPI_Wtime();  
	} 
	stats(&testParams); 

  MPI_Finalize(); 
#ifndef NDEBUG
  printf("MPI finalize\n"); 
#endif   
	
	if(rank == 0)	
	{
		deleteFile(&testParams ); 
	} 

  free(data); 
  data = NULL; 
  return 0; 
} 

