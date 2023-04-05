#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "../include/iocomp.h"

void mpiiowrite(double* iodata, struct iocomp_params *iocompParams)
{   
	int			i, j, k, initialized, finalized,ierr, size, comm, rank, argc, nprocs, myrank, nints; 

	int			count[iocompParams->NDIM],
					dims[iocompParams->NDIM],
					coords[iocompParams->NDIM], 
					periods[iocompParams->NDIM],
					order = 0, 
					disp = 0; // number of bytes to be skipped from the start. ex headers. 

#ifndef NDEBUG   
	printf("mpiWrite -> MPI variables init completed\n"); 
#endif 

	// MPI initialisations
	MPI_File        fh; 
	MPI_Status      status;
	MPI_Datatype    filetype, mpi_subarray; 

	MPI_Comm_size(iocompParams->cartcomm, &nprocs);
	MPI_Comm_rank(iocompParams->cartcomm, &myrank);
	MPI_Cart_get(iocompParams->cartcomm, iocompParams->NDIM, dims, periods, coords); 
	ierr = MPI_File_open(iocompParams->cartcomm, iocompParams->FILENAMES[iocompParams->ioLibNum],
			MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh); 
	mpi_error_check(ierr); 
	MPI_Info info  = MPI_INFO_NULL; 

	// Initialise int arrays for MPIIO 
	int localArray[iocompParams->NDIM]; 
	int globalArray[iocompParams->NDIM]; 
	int arrayStart[iocompParams->NDIM]; 
	
	for(i = 0; i < iocompParams->NDIM; i++)
	{
		localArray[i] =	 (int)iocompParams->localArray[i]; 
		globalArray[i] = (int)iocompParams->globalArray[i]; 
		arrayStart[i] =  (int)iocompParams->arrayStart[i]; 
		assert(localArray[i] > 0); 
		assert(globalArray[i] > 0); 
	}

#ifndef NDEBUG   
	printf("mpiWrite -> globalArray for rank %i \n",myrank); 
	for (int i = 0; i < iocompParams->NDIM; i++){printf("%i ", globalArray[i]); }
	printf("\nmpiWrite -> localArray for rank %i \n",myrank); 
	for (int i = 0; i < iocompParams->NDIM; i++){printf("%i ", localArray[i]); }
	printf("\nmpiWrite -> arrayStart for rank %i \n",myrank); 
	for (int i = 0; i < iocompParams->NDIM; i++){printf("%i ", arrayStart[i]); }
	printf("\n"); 
#endif 

	ierr = MPI_Type_create_subarray(iocompParams->NDIM, globalArray, localArray, arrayStart,
			MPI_ORDER_C, MPI_DOUBLE, &filetype); 
	mpi_error_check(ierr); 
#ifndef NDEBUG   
	printf("mpiWrite ->MPI create subarray \n"); 
#endif                                   

	ierr = MPI_Type_commit(&filetype); 
	mpi_error_check(ierr); 
#ifndef NDEBUG   
	printf("mpiWrite ->MPI type commit \n"); 
#endif       

	// Open file
	ierr = MPI_File_open(iocompParams->cartcomm, iocompParams->FILENAMES[iocompParams->ioLibNum], 
					MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh); 
	mpi_error_check(ierr); 
#ifndef NDEBUG   
	printf("mpiWrite ->MPI file open \n"); 
#endif       

	// Set view for this process using datatype 
	ierr = MPI_File_set_view(fh, 0, MPI_DOUBLE, filetype, "native",  MPI_INFO_NULL); 
	mpi_error_check(ierr); 
#ifndef NDEBUG   
	printf("mpiWrite ->MPI file set view \n"); 
#endif       

	int total_data = 1; 
	for (i = 0; i< iocompParams->NDIM; i++)
	{
		total_data *= localArray[i]; 
	}
	ierr = MPI_File_write_all(fh, iodata, total_data, MPI_DOUBLE, &status);   
	mpi_error_check(ierr); 
#ifndef NDEBUG   
	printf("mpiWrite ->MPI file write all \n"); 
#endif       

	ierr = MPI_File_close(&fh);
	mpi_error_check(ierr); 
#ifndef NDEBUG   
	printf("mpiWrite ->MPI file close \n"); 
#endif       

	ierr = MPI_Type_free(&filetype); 
	mpi_error_check(ierr); 
#ifndef NDEBUG   
	printf("mpiWrite ->MPI filetype\n"); 
#endif       
}


