#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

#define ioColour 0 
#define compColour 1 

void intercomm_create(struct iocomp_params *iocompParams)
{
			/*
			 * Intercomm created linking computeComm to globalComm via
			 * interComm
			 */ 

#ifndef NDEBUG
	printf("intercomm_create started for color %i \n", iocompParams->colour ); 
#endif

			int local_leader, remote_leader, ierr, inter_tag, flag;
			flag = 0;
			ierr = 0;

			int globalRank, globalSize; 

			MPI_Request request; 
			ierr = MPI_Comm_rank(iocompParams->globalComm, &globalRank); 
			mpi_error_check(ierr); 
			ierr = MPI_Comm_size(iocompParams->globalComm, &globalSize); 
			mpi_error_check(ierr); 


//			MPI_Comm splitComm; 

//			ierr = MPI_Comm_split(iocompParams->globalComm, iocompParams->colour, globalRank,	&splitComm);  // splitcommunicator based on color
//			mpi_error_check(ierr);
//#ifndef NDEBUG
//			printf("MPI comm split \n"); 
//#endif

//		printf("before mpi comm dup \n");
//			ierr = MPI_Comm_dup(splitComm, &iocompParams->compServerComm); // compute communicator for compute tasks and color != 0 
//			mpi_error_check(ierr); 
//#ifndef NDEBUG
//			printf("MPI comm dup \n"); 
//#endif

			//if (iocompParams->colour == compColour) 
			//{
				local_leader = 0; 
				remote_leader = 1 - iocompParams->colour; 
				inter_tag = 0; 
				ierr = MPI_Intercomm_create(iocompParams->compServerComm, local_leader,
						iocompParams->globalComm, remote_leader, inter_tag, &iocompParams->interComm);
				mpi_error_check(ierr); 

#ifndef NDEBUG
	printf("MPI comm test inter \n");
#endif
				ierr = MPI_Comm_test_inter(iocompParams->interComm, &flag);  // testing for intercommunicator 
				mpi_error_check(ierr); 

				if (flag == 0)
				{
					printf("intercom test fails %i \n", flag); 
				}
#ifndef NDEBUG
	printf("intercomm created for compServer \n");
#endif
			//} 

			/*
			 * Intercomm created linking ioServeComm to globalComm via
			 * interComm
			 */ 
			
			//else if (iocompParams->colour == ioColour)
			//{
				local_leader = 0; 
				remote_leader = 1 - iocompParams->colour;  
				ierr = 0; 
				inter_tag = 0; 

				ierr =MPI_Intercomm_create(iocompParams->ioServerComm, local_leader,
						iocompParams->globalComm, remote_leader, inter_tag, &iocompParams->interComm);
				mpi_error_check(ierr); 

				flag = 0;

				ierr = MPI_Comm_test_inter(iocompParams->interComm, &flag);
				mpi_error_check(ierr); 

				if (flag == 0)
				{
					printf("intercom test fails %i \n", flag); 
				}   
#ifndef NDEBUG
	printf("intercomm created for iocolour  \n"); 
#endif
			//} 
}
