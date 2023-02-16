#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "test.h"
#define LOOP_COUNT 1
#define NDIM 2 

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
	
	int localArraySize[NDIM] = {10,10}; 
	size_t localDataSize = 1; 
	for(int j = 0; j < NDIM; j++)
	{
		localDataSize*=localArraySize[j]; 
	}

	int HT_flag = 1; 
	int IOLIBNUM = 3; 

	/*
	 * initialise test function and populate iocompParams, returning data array to write. 
	 */ 
	double* data = initialise(&iocompParams,&testParams,NDIM, localArraySize, comm); 

	/*
	 * iocompInit function takes in data, and initialises ioServer 
	 */ 
  MPI_Comm computeComm = iocompInit(&iocompParams, comm,  HT_flag, IOLIBNUM); 
	// arrayParamsInit(&iocompParams,comm,NDIM,localArraySize);
	MPI_Request request; 
	
	/* 
	 * Send data multiple times, output timings and other data to a file 
	 */
	for(int i = 0; i < LOOP_COUNT; i++)
	{
		testParams.startTime[i] = MPI_Wtime();  
		dataSend(data,&iocompParams,&request,localDataSize); // send data off using dataSend
		dataWait(&iocompParams,&request);
		testParams.endTime[i] = MPI_Wtime();  
	} 
	stopSend(&iocompParams); 
	stats(&testParams); 

  MPI_Finalize(); 
#ifndef NDEBUG
  printf("MPI finalize\n"); 
#endif   
	
	if(rank == 0)	
	{
//		deleteFile(&testParams ); 
		printf("\n");
	} 

  free(data); 
  data = NULL; 
  return 0; 
} 

