#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "test.h"
#define LOOP_COUNT 1
#define MAXIO 2
#define SIZE 4
int main(int argc, char** argv)
{

  struct iocomp_params iocompParams; // struct for iocomp library 
	struct test_variables testParams; // struct for test program
	double* data; 
	int globalRank, computeRank; 

	// 0 for non splitting and 1 for splitting of comms 
	for(int HT_flag = 1; HT_flag < 2; HT_flag++)
	{
#ifndef NDEBUG
	printf("HT flag %i \n", HT_flag); 
#endif 
		for(int io = 0; io < MAXIO; io++)
		{
			int ierr;
			ierr = MPI_Init(&argc, &argv);  
			mpi_error_check(ierr); 

			MPI_Comm comm; 
			MPI_Comm_dup(MPI_COMM_WORLD,&comm); 
			int computeRank, globalRank; 
			MPI_Comm_rank(comm, &globalRank); 
			MPI_Request request; 
			// initialise array sizes
			testParams.localDataSize = pow(SIZE,2); 
#ifndef NDEBUG
	printf("IO library %i \n", io); 
#endif 
			MPI_Comm computeComm = iocompInit(&iocompParams, comm,  HT_flag, io); 
			MPI_Comm_rank(computeComm, &computeRank); 
#ifndef NDEBUG
	printf("iocomp initialised \n"); 
#endif 
			data = initialise(&testParams); // initialise array 
			for(int i = 0; i < LOOP_COUNT; i++) // send data, wait for data, check for data and loop
			{
				testParams.startTime[i] = MPI_Wtime();  
				dataSend(data,&iocompParams,&request,testParams.localDataSize); // send data off using dataSend
#ifndef NDEBUG
	printf("data send\n"); 
#endif 
				dataWait(&iocompParams,&request);
				testParams.endTime[i] = MPI_Wtime(); 
				//check_data(io,localDataSize); 
			} 
			//print_stats(&testParams); 
			stopSend(&iocompParams); // stop iocomp
//			if(!globalRank){
//				deleteFile(&testParams, io); // delete file after I/O ops 
//			 } 
			MPI_Finalize(); 
		}	
	} 
  
  free(data); 
  data = NULL; 
	return 0; 
} 


