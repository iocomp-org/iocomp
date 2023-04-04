#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include <adios2_c.h>
#include <adios2/c/adios2_c_types.h>
#include "../include/iocomp.h"
#define config_file "config.xml"

#define fullNode 256 

/*
 * Initialises the library 
 */
void ioServerInitialise(struct iocomp_params *iocompParams, int ioLibNum)
{
	iocompParams->ioLibNum = ioLibNum; 
#ifndef NDEBUG
			printf("ioServerInitialise -> start with flag %i \n", iocompParams->hyperthreadFlag); 
#endif

	/*
	 * Single initialisation of I/O things 
	 */ 
	int dims_mpi[iocompParams->NDIM]; 		
	int periods[iocompParams->NDIM]; 		
	int coords[iocompParams->NDIM]; 		
	double writeTime = 0.0; 
	
	int ioSize, ioRank; 
	int reorder = 0; 
	MPI_Comm_size(iocompParams->ioServerComm, &ioSize); 
	MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
	/* new communicator to which topology information is added */
	// MPI_Comm cartcomm;
#ifndef NDEBUG
	printf("ioLibraries -> MPI new cartcomm \n");
#endif

	MPI_Dims_create(ioSize, iocompParams->NDIM, dims_mpi);
#ifndef NDEBUG
	printf("ioLibraries -> MPI dims create \n");
#endif

	MPI_Cart_create(iocompParams->ioServerComm, iocompParams->NDIM, dims_mpi, periods, reorder, &iocompParams->cartcomm); // comm;
#ifndef NDEBUG
	printf("ioLibraries -> MPI cart create  \n");
#endif

	MPI_Cart_coords(iocompParams->cartcomm, ioRank, iocompParams->NDIM, coords);
#ifndef NDEBUG
	printf("ioLibraries -> MPI cart coords \n");
#endif

	/*
	 * adios2 object declared before loop entered
	 * only when ioLib chosen is adios2 and one of its engines 
	 */ 
	iocompParams->adios = adios2_init_config_mpi(config_file, cartcomm); // cartcomm); // if using ADIOS2 MPI, need to include debugger. 
	if(iocompParams->ioLibNum >=2 && iocompParams->ioLibNum <= 4)
	{
		iocompParams->adios = adios2_init_config_mpi(config_file, iocompParams->cartcomm); 
	} 

	if(iocompParams->hyperthreadFlag)
	{
		if(iocompParams->colour == ioColour)
		{
#ifndef NDEBUG
			printf("ioServerInitialise -> ioServer called\n"); 
#endif
			ioServer(iocompParams);
#ifndef NDEBUG
			printf("ioServerInitialise -> After ioServer\n"); 
#endif
			MPI_Finalize(); 

#ifndef NDEBUG
			printf("ioServerInitialise -> After finalize\n"); 
#endif
			exit(0); 
		} 
	}

} 

