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
void ioServerInitialise(struct iocomp_params *iocompParams)
{
#ifndef NDEBUG
	printf("ioServerInitialise -> start with flag %i \n", iocompParams->hyperthreadFlag); 
#endif

	/*
	 * Initialisations for MPI cartesian communicator 
	 */ 
	int dims_mpi[iocompParams->NDIM]; 		
	int periods[iocompParams->NDIM]; 		
	int coords[iocompParams->NDIM]; 		
	double writeTime = 0.0; 
	for (int j = 0; j < iocompParams->NDIM; j++)
	{
		dims_mpi[j] = 0;
		periods[j] = 0;
		coords[j] = 0;
	}

	/*
	 * MPI ranks and size 
	 */ 
	int ioSize, ioRank, ierr; 
	int reorder = 0; 
	ierr = MPI_Comm_size(iocompParams->ioServerComm, &ioSize); 
	mpi_error_check(ierr); 
	ierr = MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
	mpi_error_check(ierr); 
#ifndef NDEBUG
	printf("ioServerInitialise -> MPI size %i and rank %i \n", ioSize, ioRank);
#endif

	/* 
	 * new communicator to which topology information is added 
	 */ 
	ierr = MPI_Dims_create(ioSize, iocompParams->NDIM, dims_mpi);
	mpi_error_check(ierr); 
#ifndef NDEBUG
	printf("ioServerInitialise-> MPI dims create \n");
#endif

	ierr = MPI_Cart_create(iocompParams->ioServerComm, iocompParams->NDIM, dims_mpi,
		periods, reorder, &iocompParams->cartcomm); // comm;
	mpi_error_check(ierr); 
#ifndef NDEBUG
	printf("ioServerInitialise -> MPI cart create  \n");
#endif

	ierr = MPI_Cart_coords(iocompParams->cartcomm, ioRank, iocompParams->NDIM, coords);
	mpi_error_check(ierr); 
#ifndef NDEBUG
	printf("ioServerInitialise -> MPI cart coords \n");
#endif

	/*
	 * define local size, global size, array offsets for ioLibraries 
	 */ 
	arrayParamsInit(iocompParams,iocompParams->ioServerComm); 

	/*	
	 * Initiliase filename 
	 */ 
	iocompParams->FILENAMES[0] = "mpiio.dat"; 
	iocompParams->FILENAMES[1] = "hdf5.h5"; 
	iocompParams->FILENAMES[2] = "adios2.h5";
	iocompParams->FILENAMES[3] = "adios2.bp4";
	iocompParams->FILENAMES[4] = "adios2.bp5"; 
	/*
	 * Initialise adios2 engines list  
	 */ 
	iocompParams->ADIOS2_IOENGINES[0] = "HDF5"; 
	iocompParams->ADIOS2_IOENGINES[1] = "BP4"; 
	iocompParams->ADIOS2_IOENGINES[2] = "BP5";

	/*
	 * adios2 object declared before loop entered
	 * only when ioLib chosen is adios2 and one of its engines 
	 */ 
	if(iocompParams->ioLibNum >=2 && iocompParams->ioLibNum <= 4)
	{
		iocompParams->adios = adios2_init_config_mpi(config_file, iocompParams->cartcomm); 
		iocompParams->io = adios2_declare_io(iocompParams->adios, 
			iocompParams->ADIOS2_IOENGINES[iocompParams->ioLibNum-2]); //IO handler declaration
	} 

	/*
	 * Initialise previous initialisation flag. 
	 */ 
	iocompParams->previousInit = 0;  

} 

