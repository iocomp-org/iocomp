#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

#define n 10  
#define NDIM 2
#define ioColor 0 
#define compColor 1 

void mpi_error_check(int ierr)
{
	if (ierr != MPI_SUCCESS)
	{
		printf("MPI call fails \n"); 
	}
}

//void compute_comm_create(int color, MPI_Comm splitComm, MPI_Comm *computeComm)
//{
//			int ierr; 
//			ierr =MPI_Comm_dup(splitComm, computeComm); // compute communicator for compute tasks and color != 0 
//			mpi_error_check(ierr); 
//
//#ifndef NDEBUG
//			printf("MPI comm dup \n"); 
//#endif
//}


void comm_split(int color, MPI_Comm globalComm, MPI_Comm *computeComm, MPI_Comm *ioServerComm)
{
			int ierr, myrank;
			MPI_Comm splitComm; 
			MPI_Comm_rank(globalComm, &myrank); 
			ierr =	MPI_Comm_split(globalComm, color, myrank,	&splitComm);  // splitcommunicator based on color 
			mpi_error_check(ierr);

#ifndef NDEBUG
			printf("MPI comm split \n"); 
#endif
			if (color == compColor) 
			{	
				ierr = MPI_Comm_dup(splitComm, computeComm); // compute communicator for compute tasks and color != 0 
				mpi_error_check(ierr); 

#ifndef NDEBUG
			printf("MPI computeComm \n"); 
#endif
			}
			
			if (color == ioColor) 
			{	
				ierr = MPI_Comm_dup(splitComm, ioServerComm); // compute communicator for compute tasks and color != 0 
				mpi_error_check(ierr); 

#ifndef NDEBUG
			printf("MPI ioServerComm \n"); 
#endif
			}
}

void intercomm_create(int color, MPI_Comm globalComm, MPI_Comm computeComm, MPI_Comm ioServeComm, MPI_Comm *interComm)
{
			/*
			 * Intercomm created linking computeComm to globalComm via
			 * interComm
			 */ 

			int local_leader, remote_leader, ierr, inter_tag, flag;
			flag = 0; 
			if (color == compColor) 
			{
				local_leader = 0; 
				remote_leader = 1 - color; 
				inter_tag = 0; 

				ierr =MPI_Intercomm_create(computeComm, local_leader,
						globalComm, remote_leader, inter_tag, interComm);
				mpi_error_check(ierr); 

				ierr =MPI_Comm_test_inter(*interComm, &flag);  // testing for intercommunicator 
				mpi_error_check(ierr); 

				if (flag == 0)
				{
					printf("intercom test fails %i \n", flag); 
				}
			} 

			/*
			 * Intercomm created linking ioServeComm to globalComm via
			 * interComm
			 */ 
			
			if (color == ioColor)
			{
				local_leader = 0; 
				remote_leader = 1 - color;  

				ierr =MPI_Intercomm_create(ioServeComm, local_leader,
						globalComm, remote_leader, inter_tag, interComm);
				mpi_error_check(ierr); 

				flag = 0;

				ierr = MPI_Comm_test_inter(*interComm, &flag);
				mpi_error_check(ierr); 

				if (flag == 0)
				{
					printf("intercom test fails %i \n", flag); 
				}   
			} 
}

void intercomm(MPI_Comm comm, double* data, int IO_SERVER_SIZE)
{
	int ierr;  
#ifndef NDEBUG
	printf("after mpi init main program \n"); 
#endif
	int myrank, compute_task = 1, io_task = 0, 
			splitCommRank, io_start = 0, comp_start = 1, color = 1, 
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
			MPI_Comm globalComm, splitComm, computeComm, ioServeComm, interComm; 

#ifndef NDEBUG
			printf("MPI comms declaration of main program \n"); 
#endif
			MPI_Request request; 
			ierr =MPI_Comm_dup(comm, &globalComm); 
			mpi_error_check(ierr); 
			ierr =MPI_Comm_rank(globalComm, &myrank); 
			mpi_error_check(ierr); 
			ierr =MPI_Comm_size(globalComm, &mysize); 
			mpi_error_check(ierr); 

#ifndef NDEBUG
			printf("MPI rank and size etc \n"); 
#endif

			if (mysize < 2) // needs to be at least 2 ranks 
			{
				printf("Need at least 2 ranks. Program quitting. \n"); 
				exit(0); 
			}

			if (myrank < IO_SERVER_SIZE ) // if rank 0, IO server and color = 0 otherwise its a compute server and color = 1 
			{
				color = ioColor; 
			}

			comm_split(color, globalComm, &computeComm, &ioServeComm); // function to split communicator based on color
			// compute_comm_create(color, globalComm, &computeComm); // function to create compute communicator 

			// initialisation of local size array and local data size 
			int compute_rank_size = mysize - IO_SERVER_SIZE; 
			for (i = 0; i < NDIM; i++)
			{
				local_size[i] = n; 
			} 

			for (i = 0; i < NDIM; i++)
			{
				local_data_size *= local_size[i]; 
			}

			global_data_size = local_data_size * compute_rank_size; 

#ifndef NDEBUG
			printf("local data size and local size initialised \n"); 
#endif

//			ierr =MPI_Comm_rank(computeComm, &compute_rank);
//			mpi_error_check(ierr); 
//			ierr =MPI_Comm_rank(computeComm, &compute_size);
//			mpi_error_check(ierr); 

#ifndef NDEBUG
			if ( color == compColor )
			{
				printf("Hello from computecomm with rank %i and color %i \n", compute_rank, color); 
			} 
#endif

			/*
			 * Intercomm created linking computeComm to globalComm via
			 * interComm
			 */ 

			intercomm_create(color, globalComm, computeComm, ioServeComm, &interComm); 
//			ierr = MPI_Comm_dup(splitComm, &ioServeComm); // IO communicator for IO tasks and color == 0
//			mpi_error_check(ierr); 
//			ierr =MPI_Comm_rank(ioServeComm, &io_rank);
//			mpi_error_check(ierr); 
//
#ifndef NDEBUG
			if ( color == ioColor )
			{
				printf("Hello from ioServeComm with rank %i and color %i \n", io_rank, color); 
			} 
#endif

//			/*
//			 * Intercomm created linking ioServeComm to globalComm via
//			 * interComm
//			 */ 
//
//			local_leader = 0; 
//			remote_leader = 1 - color;  
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
			if(color == compColor) // Compute task 
			{
				computeServer(NDIM, data, local_size, computeComm, interComm); 
			}

			else if (color == ioColor) // IO task 
			{
				ioServer(NDIM, local_size, interComm, ioServeComm, globalComm);
			}        

/*			MPI_Comm_free(&splitComm); 
			MPI_Comm_free(&computeComm); 
			MPI_Comm_free(&interComm); 
			MPI_Comm_free(&globalComm); 
		MPI_Comm_free(&ioServeComm); 
		*/ 







}
