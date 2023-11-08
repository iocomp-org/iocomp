#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"
#define config_file "config.xml"

// include ADIOS2 files if flag not defined 
#ifndef NOADIOS2 
#include <adios2_c.h>
#include <adios2/c/adios2_c_types.h>
#endif 

/*
 * Initialises the library 
 */
void ioServerInitialise(struct iocomp_params *iocompParams)
{
	/*
	 * MPI ranks and size 
	 */ 
	int ioSize, ioRank, ierr; 
	int reorder = 0; 
	ierr = MPI_Comm_size(iocompParams->ioServerComm, &ioSize); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
	mpi_error_check(ierr); 

#ifdef VERBOSE
	fprintf(iocompParams->debug,"ioServerInitialise -> MPI size %i and rank %i \n", ioSize, ioRank);
#endif

	/*
	 * Initialisations for MPI cartesian communicator 
	 */ 
	int dims_mpi[iocompParams->NDIM]; 		
	int periods[iocompParams->NDIM]; 		
	int coords[iocompParams->NDIM]; 		
	for (int j = 0; j < iocompParams->NDIM; j++)
	{
		dims_mpi[j] = 0;
		periods[j] = 0;
		coords[j] = 0;
	}

	/* 
	 * new communicator to which topology information is added 
	 */ 
	ierr = MPI_Dims_create(ioSize, iocompParams->NDIM, dims_mpi);
	mpi_error_check(ierr); 
#ifdef VERBOSE
	fprintf(iocompParams->debug,"ioServerInitialise-> MPI dims create \n");
#endif

	ierr = MPI_Cart_create(iocompParams->ioServerComm, iocompParams->NDIM, dims_mpi,
			periods, reorder, &iocompParams->cartcomm); // comm;
	mpi_error_check(ierr); 
#ifdef VERBOSE
	fprintf(iocompParams->debug,"ioServerInitialise -> MPI cart create  \n");
#endif

	ierr = MPI_Cart_coords(iocompParams->cartcomm, ioRank, iocompParams->NDIM, coords);
	mpi_error_check(ierr); 
#ifdef VERBOSE
	fprintf(iocompParams->debug,"ioServerInitialise -> MPI cart coords \n");
#endif

	/*
	 * Initialise adios2 engines list  
	 */ 
	iocompParams->ADIOS2_IOENGINES[0] = "HDF5"; 
	iocompParams->ADIOS2_IOENGINES[1] = "BP4"; 
	iocompParams->ADIOS2_IOENGINES[2] = "BP5";

#ifdef VERBOSE
	fprintf(iocompParams->debug,"ioServerInitialise -> assigned file names and adios2 ioengines\n");
#endif

#ifndef NOADIOS2
	/*
	 * adios2 object declared before loop entered
	 * only when ioLib chosen is adios2 and one of its engines 
	 */ 
	if(iocompParams->ioLibNum >=2 && iocompParams->ioLibNum <= 4)
	{
#if ADIOS2_USE_MPI
		iocompParams->adios = adios2_init_config_mpi(config_file, iocompParams->cartcomm); 
#else 
		iocompParams->adios = adios2_init();  
#endif 
		iocompParams->io = adios2_declare_io(iocompParams->adios, 
				iocompParams->ADIOS2_IOENGINES[iocompParams->ioLibNum-2]); //IO handler declaration
#ifdef VERBOSE
		fprintf(iocompParams->debug,"ioServerInitialise -> initialised adios2 engine and io param\n");
#endif
	} 
#endif 

	/*
	 * Initialise previous initialisation and element counter flag
	 */ 
	iocompParams->previousInit = 0;  
	iocompParams->previousCount = 0;  
	iocompParams->adios2Init = 0;  
#ifdef VERBOSE
	fprintf(iocompParams->debug,
			"ioServerInitialise -> initialisation flags set, previousInit %i, previousCount %i, adios2Init %i\n", 
			iocompParams->previousInit, iocompParams->previousCount, iocompParams->adios2Init);
#endif


} 

