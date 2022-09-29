#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

#define n 10  
#define NDIM 2
#define ioColour 0 
#define compColour 1 

void mpi_error_check(int ierr)
{
	if (ierr != MPI_SUCCESS)
	{
		printf("MPI call fails \n"); 
	}
}

void intercomm(MPI_Comm comm, double* data, struct iocomp_params *iocompParams )
{
	int ierr;  
#ifndef NDEBUG
	printf("after mpi init main program \n"); 
#endif
	int myrank, compute_task = 1, io_task = 0, 
			splitCommRank, io_start = 0, comp_start = 1, 
			myrank2, myrank3, myrank4, compute_rank, compute_size, io_rank, flag = 0, i, test_val, mysize, PROCS, dest, tag,
			inter_tag, local_leader, remote_leader, N1, N2, size, local_array;

	int size_ar[NDIM];
	int local_size[NDIM], 
			global_size[NDIM],
			arraystart[NDIM],
			local_data_size = 1, 
			global_data_size = 1; 

#ifndef NDEBUG
			printf("MPI comms declaration of main program \n"); 
#endif
			MPI_Request request; 
			ierr =MPI_Comm_dup(comm, &iocompParams->globalComm); 
			mpi_error_check(ierr); 
			ierr =MPI_Comm_rank(iocompParams->globalComm, &myrank); 
			mpi_error_check(ierr); 
			ierr =MPI_Comm_size(iocompParams->globalComm, &mysize); 
			mpi_error_check(ierr); 

#ifndef NDEBUG
			printf("MPI rank and size etc \n"); 
#endif

			comm_split(iocompParams); // function to split communicator based on colour assigned in intercomm_init. 
			
			for (i = 0; i < NDIM; i++)
			{
				local_size[i] = n; 
			} 

			for (i = 0; i < NDIM; i++)
			{
				local_data_size *= local_size[i]; 
			}

			global_data_size = local_data_size * iocompParams->compServerSize; 

#ifndef NDEBUG
			printf("local data size and local size initialised \n"); 
#endif

			/*
			 * Intercomm created linking computeComm to globalComm via
			 * interComm
			 */ 

			intercomm_create(iocompParams); 

#ifndef NDEBUG
			int ioRank; 
			MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
			if ( iocompParams->colour == ioColour )
			{
				printf("Hello from ioServeComm with rank %i and colour %i \n", ioRank, iocompParams->colour); 
			} 
			
			int computeRank; 
			MPI_Comm_rank(iocompParams->compServerComm, &computeRank); 
			if ( iocompParams->colour == compColour )
			{
				printf("Hello from computecomm with rank %i and colour %i \n", compute_rank, iocompParams->colour); 
			} 
#endif

			/*
			 * Send data to ioServer and ComputerServer
			 */ 

			if(iocompParams->colour == compColour) // Compute task 
			{
				computeServer(NDIM, data, local_size, iocompParams) ; 
			}

			else if (iocompParams->colour == ioColour) // IO task 
			{
				ioServer(NDIM, local_size, iocompParams);
			}        

			MPI_Comm_free(&iocompParams->compServerComm); 
			MPI_Comm_free(&iocompParams->interComm); 
			MPI_Comm_free(&iocompParams->globalComm); 
		  MPI_Comm_free(&iocompParams->ioServerComm); 

}
