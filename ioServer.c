#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"
#define filename "write_time.csv"
void ioServer(int NDIM, int* local_size, MPI_Comm interComm, MPI_Comm ioServeComm, MPI_Comm globalComm)
{

	int i, ierr, globalRank, globalSize, computeRankSize; 
	int ioRank, ioSize; 
	int global_size[NDIM],
	array_start[NDIM]; 
	double timer_start, timer_end; 
	timer_start = 0; 
	timer_end = 0; 

	ierr = MPI_Comm_rank(globalComm, &globalRank); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_size(globalComm, &globalSize); 
	mpi_error_check(ierr); 

	ierr = MPI_Comm_rank(ioServeComm, &ioRank); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_size(ioServeComm, &ioSize); 
	mpi_error_check(ierr); 

	MPI_Status status;
	MPI_Request request; 
	MPI_Info info;  

	computeRankSize = globalSize - 1; // Assumption that only one rank in IO server 

	int globalDataSize = 1; 
	int localDataSize= 1; 

	/*
	 * Initialisation of localsize, global size, localdatasize and globaldatasize
	 */ 
	for (i = 0; i< NDIM; i++)
	{
		global_size[i] = local_size[i]; 
		localDataSize *= local_size[i]; 
	}

	global_size[0] = local_size[0]*computeRankSize; 

	for (i = 0; i< NDIM; i++)
	{
		globalDataSize *= global_size[i]; 
	}


#ifndef NDEBUG
		printf("globaldatasize %i localdatasize %i \n", globalDataSize, localDataSize); 
#endif


	// initialise recv array 
	double* recv = NULL; 
	recv = (double*)malloc(globalDataSize*sizeof(double)); 


#ifndef NDEBUG
		printf("Initialisation of recv array \n"); 
#endif

	for ( i = 0; i < globalDataSize; i++)
	{
		recv[i] = 0; // initialise recv array 
	}

	/*
	 * Go to each compute rank and get data from it 
	 * Assuming compute ranks are total ranks - 1 
	 */ 

	int computeRank = 0; 

#ifndef NDEBUG
		printf("For loop for computeRanks recieving starts  \n"); 
#endif

	for ( computeRank = 0; computeRank < computeRankSize; computeRank ++) 
	{

		/*
		 * Assign arraystart position for writing of array
		 * Assuming weak scaling. Outerdimension would have n*totalrank
		 */ 


		for(int i = 0; i < NDIM; i++)
		{
			array_start[i] = 0; 
		}
		array_start[0] = computeRank * local_size[0]; // assuming ar_size has uniform dimensions. 

#ifndef NDEBUG
		printf("Recieving data starts \n"); 
#endif

		ierr = MPI_Irecv(recv, localDataSize, MPI_DOUBLE, computeRank,
				computeRank, interComm, &request); 
		mpi_error_check(ierr); 

#ifndef NDEBUG
		printf("Irecv completed \n");
#endif

		ierr = 	MPI_Waitall(1, &request, &status); // wait for all processes to finish sending and recieving  
		mpi_error_check(ierr); 

#ifndef NDEBUG
		printf("Recv data coming from rank %i \n",computeRank); 
		for(i = 0; i < localDataSize; i++)
		{
			printf("%lf ",recv[i]); // init size in each dimension to be n. For ex. NDIM = 2 will reslt in n x n 
		}
		printf("\n"); 
#endif
		if (ioRank == 0) // timing will be measured by using ioRank = 0 
		{	
			timer_start = MPI_Wtime();
		}
		// bench_init(recv, NDIM, local_size, global_size, array_start, ioServeComm ); 
		MPI_Barrier(ioServeComm); // Wait for all processes to finish  

#ifndef NDEBUG
		printf("MPI barrier \n"); 
#endif

		if (ioRank == 0)
		{
			timer_end  = MPI_Wtime();
		} 
	} 

	free(recv);
	recv = NULL; 

	if (ioRank == 0) 
	{
		int test; 
		FILE* out; 
		double write_time = 0; 
		write_time = timer_end - timer_start; 

#ifndef NDEBUG
		printf("remove filename \n");
#endif
		test = remove(filename);
		if (test != 0)
		{
#ifndef NDEBUG
			printf("Cant remove %s \n", filename);
#endif 
		}
		out = fopen(filename, "w+");
		if (out == NULL)
		{
			printf("Error: No output file\n");
			exit(1);
		}

    fprintf(out, "Time(s), LocalDataSize(B), GlobalDataSize(B) \n"); //headers for output csv 
    fprintf(out, "%lf,%ld,%ld \n", write_time, localDataSize*sizeof(double), globalDataSize*sizeof(double)); //headers for output csv 
	}



} 
