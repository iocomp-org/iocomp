#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

#define ioColour 0 
#define compColour 1 

void mpi_error_check(int ierr)
{
	if (ierr != MPI_SUCCESS)
	{
		printf("MPI call fails \n"); 
	}
}

void intercomm( struct iocomp_params *iocompParams )
{
	int ierr;  
	/*
	 * split communicators based on colour assigned in intercommInit 
	 */ 

	//comm_split(iocompParams);  

	/*
	 * Intercomm created linking computeComm to globalComm via
	 * interComm
	 */ 

	//intercomm_create(iocompParams); 

#ifndef NDEBUG
	int ioRank, computeRank; 
	MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
	MPI_Comm_rank(iocompParams->compServerComm, &computeRank); 
	if ( iocompParams->colour == ioColour )
		printf("Hello from ioServeComm with rank %i and colour %i \n", ioRank, iocompParams->colour); 
	else if ( iocompParams->colour == compColour )
		printf("Hello from computecomm with rank %i and colour %i \n", computeRank, iocompParams->colour); 
#endif

	/*
	 * Send data to ioServer and ComputerServer
	 */ 
	
//	if(iocompParams->colour == compColour) // Compute task 
//	{
//		computeServer(data, iocompParams); 
//#ifndef NDEBUG
//		printf("After computeServer\n"); 
//#endif
//	}
//
//	else if (iocompParams->colour == ioColour) // IO task 
//	{
//		ioServer(iocompParams);
//#ifndef NDEBUG
//		printf("After ioServer\n"); 
//#endif
//	} 
//
		if (iocompParams->colour == ioColour) // IO task 
		
		{
			ioServer(iocompParams);
		 
#ifndef NDEBUG
		printf("After ioServer\n"); 
#endif
		} 
	/*
	 * Free communicators 
	 */ 

	//if(iocompParams->colour == compColour) // Compute task 
	//{
	//	MPI_Comm_free(&iocompParams->compServerComm); 
	//}

	//else if (iocompParams->colour == ioColour) // IO task 
	//{
	//MPI_Comm_free(&iocompParams->ioServerComm); 
	//MPI_Comm_free(&iocompParams->interComm); 
	//MPI_Comm_free(&iocompParams->globalComm); 
}
