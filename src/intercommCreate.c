#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

#define ioColour 0 
#define compColour 1 

void intercomm_create(struct iocomp_params *iocompParams)
{

#ifndef NDEBUG
	printf("intercomm_create started for color %i \n", iocompParams->colour ); 
#endif

	int local_leader, remote_leader, ierr, inter_tag, flag;
	flag = 0;
	ierr = 0;

	int globalMPIRank, globalMPISize; 

	MPI_Request request; 
	ierr = MPI_Comm_rank(iocompParams->globalComm, &globalMPIRank); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_size(iocompParams->globalComm, &globalMPISize); 
	mpi_error_check(ierr); 

	if (iocompParams->colour == compColour) // if CompServer then create intercomm
	{
#ifndef NDEBUG
		printf("before creating compServerComm\n");
#endif
		local_leader = 0; 
		remote_leader = globalMPISize/2; // remote leader in IO server in globalComm 
		inter_tag = 1; 
		ierr = MPI_Intercomm_create(iocompParams->compServerComm, local_leader,
				iocompParams->globalComm, remote_leader, inter_tag, &iocompParams->interComm); // compServer communicates to group 0 i.e. IO server
		mpi_error_check(ierr); 

		flag = 0 ; 
		ierr = MPI_Comm_test_inter(iocompParams->interComm, &flag);  // testing for intercommunicator 
		mpi_error_check(ierr); 

		if (flag == 0)
		{
			printf("intercom test fails %i \n", flag); 
		}
#ifndef NDEBUG
		printf("intercomm created for compServer \n");
#endif
	} 

	/*
	 * Intercomm created linking ioServeComm to globalComm via
	 * interComm
	 */ 

	else if (iocompParams->colour == ioColour) // if ioServer then create intercomm
	{
#ifndef NDEBUG
		printf("before creating ioServerComm\n");
#endif
		local_leader = 0; 
		remote_leader = 0; // remote leader of compute servers in global Comm  
		inter_tag = 1;   

		ierr = MPI_Intercomm_create(iocompParams->ioServerComm, local_leader,
				iocompParams->globalComm, remote_leader, inter_tag, &iocompParams->interComm); // ioServer Communicates to group 1 i.e. CompServer
		mpi_error_check(ierr); 

		flag = 0;

		ierr = MPI_Comm_test_inter(iocompParams->interComm, &flag);
		mpi_error_check(ierr); 

		if (flag == 0)
		{
			printf("intercom test fails %i \n", flag); 
		}   

#ifndef NDEBUG
		printf("intercomm created for ioServer  \n"); 
#endif
	} 

	// testing for intercomms 
	int interRank, interSize; 
	MPI_Comm_rank(iocompParams->interComm, &interRank); 
	MPI_Comm_size(iocompParams->interComm, &interSize); 
	if(interRank == 0)
	{
		printf("Intercomms declared, colour %i has size %i \n",iocompParams->colour, interSize ); 
	}
}
