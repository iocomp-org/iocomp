#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>
#include <memory.h>
#include "../include/iocomp.h"

void mpiRead(double *readData, char* FILENAME, struct iocomp_params *iocompParams)
{
	int i, ierr, nprocs, myrank; 

	int dims[iocompParams->NDIM],
			coords[iocompParams->NDIM], 
			periods[iocompParams->NDIM]; 

	// convert size_t array parameters to int arrays using MPI and HDF5 
	int localArray[iocompParams->NDIM]; 
	int globalArray[iocompParams->NDIM]; 
	int arrayStart[iocompParams->NDIM]; 
	for(int i = 0; i < iocompParams->NDIM; i++)
	{
		localArray[i] = (int)iocompParams->localArray[i]; 
		globalArray[i] = (int)iocompParams->globalArray[i]; 
		arrayStart[i] = (int)iocompParams->arrayStart[i]; 
	}

	// MPI initialisations
	MPI_File        fh; 
	MPI_Status      status;
	MPI_Datatype    filetype; 

	MPI_Comm_size(iocompParams->cartcomm, &nprocs);
	MPI_Comm_rank(iocompParams->cartcomm, &myrank);

	ierr = MPI_Cart_get(iocompParams->cartcomm, iocompParams->NDIM, dims, periods, coords); 
	mpi_error_check(ierr); 

	ierr = MPI_File_open(iocompParams->cartcomm, FILENAME,
			MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh); 
	mpi_error_check(ierr); 

#ifdef VERBOSE   
	fprintf(iocompParams->debug,"Cartcomm rank and size %i %i \n", myrank, nprocs); 
	fprintf(iocompParams->debug,"arraygsize for rank %i : %i  \n",myrank, globalArray[0]	); 
	fprintf(iocompParams->debug,"arraysubsize for rank %i : %i \n",myrank,localArray[0]); 
	fprintf(iocompParams->debug,"arraystart for rank %i : %i  \n",myrank,arrayStart[0]		); 
#endif 

	ierr = MPI_Type_create_subarray(iocompParams->NDIM, globalArray, localArray, arrayStart,
			MPI_ORDER_C, MPI_DOUBLE, &filetype); 
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"MPI create subarray \n"); 
#endif                                   

	ierr = MPI_Type_commit(&filetype); 
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"MPI type commit \n"); 
#endif       

	ierr = MPI_File_open(iocompParams->cartcomm, FILENAME, MPI_MODE_RDONLY, 
			MPI_INFO_NULL, &fh); 
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"MPI file open \n"); 
#endif       

	// Set view for this process using datatype 
	ierr = MPI_File_set_view(fh, 0, MPI_DOUBLE, filetype, "native",  
			MPI_INFO_NULL); 
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"MPI file set view \n"); 
#endif       

	// Remove halo data    
	int total_data = 1; 
	for (i = 0; i< iocompParams->NDIM; i++)
	{
		total_data *= localArray[i]; 
	}
	ierr = MPI_File_read_all(fh, readData, total_data, MPI_DOUBLE, &status);   
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"MPI file write all \n"); 
#endif       

	ierr = MPI_File_close(&fh);
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"MPI file close \n"); 
#endif       

	ierr = MPI_Type_free(&filetype); 
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"MPI filetype\n"); 
#endif       

}


