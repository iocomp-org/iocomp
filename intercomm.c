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

//void compute_comm_create(int colour, MPI_Comm splitComm, MPI_Comm *computeComm)
//{
//			int ierr; 
//			ierr =MPI_Comm_dup(splitComm, computeComm); // compute communicator for compute tasks and colour != 0 
//			mpi_error_check(ierr); 
//
//#ifndef NDEBUG
//			printf("MPI comm dup \n"); 
//#endif
//}


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

			/*
			 * MPI initialisations 
			 */ 
			// MPI_Comm globalComm, splitComm, computeComm, ioServeComm, interComm; 

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

		//	if (myrank < IO_SERVER_SIZE ) // if rank 0, IO server and colour = 0 otherwise its a compute server and colour = 1 
		//	{
		//		iocompParams->colour = ioColour; 
		//	}

			comm_split(iocompParams); // function to split communicator based on colour
			// compute_comm_create(colour, globalComm, &computeComm); // function to create compute communicator 

			// initialisation of local size array and local data size 
//			int compute_rank_size = mysize - IO_SERVER_SIZE; 
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

//			ierr =MPI_Comm_rank(computeComm, &compute_rank);
//			mpi_error_check(ierr); 
//			ierr =MPI_Comm_rank(computeComm, &compute_size);
//			mpi_error_check(ierr); 

#ifndef NDEBUG
			int computeRank; 
			MPI_Comm_rank(iocompParams->compServerComm, &computeRank); 
			if ( iocompParams->colour == compColour )
			{
				printf("Hello from computecomm with rank %i and colour %i \n", compute_rank, iocompParams->colour); 
			} 
#endif

			/*
			 * Intercomm created linking computeComm to globalComm via
			 * interComm
			 */ 

			intercomm_create(iocompParams); 
//			ierr = MPI_Comm_dup(splitComm, &ioServeComm); // IO communicator for IO tasks and colour == 0
//			mpi_error_check(ierr); 
//			ierr =MPI_Comm_rank(ioServeComm, &io_rank);
//			mpi_error_check(ierr); 
//
#ifndef NDEBUG
			int ioRank; 
			MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
			if ( iocompParams->colour == ioColour )
			{
				printf("Hello from ioServeComm with rank %i and colour %i \n", ioRank, iocompParams->colour); 
			} 
#endif

//			/*
//			 * Intercomm created linking ioServeComm to globalComm via
//			 * interComm
//			 */ 
//
//			local_leader = 0; 
//			remote_leader = 1 - colour;  
//
//			ierr =MPI_Intercomm_create(ioServeComm, local_leader,
//					globalComm, remote_leader, inter_tag, &interComm);
//			mpi_error_check(ierr); 
//
//			flag = 0;
//
//			ierr = MPI_Comm_test_inter(interComm, &flag);
//			mpi_error_check(ierr); 
//
//			if (flag == 0)
//			{
//				printf("intercom test fails %i \n", flag); 
//			}   
//
			if(iocompParams->colour == compColour) // Compute task 
			{
				computeServer(NDIM, data, local_size, iocompParams) ; 
			}

			else if (iocompParams->colour == ioColour) // IO task 
			{
				ioServer(NDIM, local_size, iocompParams);
			}        

/*			MPI_Comm_free(&splitComm); 
			MPI_Comm_free(&computeComm); 
			MPI_Comm_free(&interComm); 
			MPI_Comm_free(&globalComm); 
		MPI_Comm_free(&ioServeComm); 
		*/ 

}
