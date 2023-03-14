#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "../include/iocomp.h"

void mpiiowrite(double* iodata, int*arraysubsize, int* arraygsize, int* arraystart, int NDIM, MPI_Comm cartcomm, char* FILENAME, MPI_Datatype dataType)
{   
	int			i, j, k, initialized, finalized,ierr, size, comm, rank, argc, nprocs, myrank, nints; 

	int			count[NDIM],
	dims[NDIM],
	coords[NDIM], 
	periods[NDIM],
	order = 0, 
	disp = 0; // number of bytes to be skipped from the start. ex headers. 

#ifndef NDEBUG   
	printf("mpiWrite -> MPI variables init completed \n"); 
#endif 

	// MPI initialisations
	MPI_File        fh; 
	MPI_Status      status;
	MPI_Datatype    filetype, mpi_subarray; 

	// MPI_Errhandler_set(cartcomm,MPI_ERRORS_RETURN); /* return info about
	//   errors */
	MPI_Comm_size(cartcomm, &nprocs);
	MPI_Comm_rank(cartcomm, &myrank);
	MPI_Cart_get(cartcomm, NDIM, dims, periods, coords); 
	ierr = MPI_File_open(cartcomm, FILENAME,
			MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh); 
	mpi_error_check(ierr); 
	// check_mpi_error(ierr, cartcomm); 
	MPI_Info info  = MPI_INFO_NULL; 

#ifndef NDEBUG   
	printf("mpiWrite -> arraygsize for rank %i \n",myrank); 
	for (int i = 0; i < NDIM; i++){printf("%i ", arraygsize[i]); }
	printf("\nmpiWrite -> arraysubsize for rank %i \n",myrank); 
	for (int i = 0; i < NDIM; i++){printf("%i ", arraysubsize[i]); }
	printf("\nmpiWrite -> arraystart for rank %i \n",myrank); 
	for (int i = 0; i < NDIM; i++){printf("%i ", arraystart[i]); }
	printf("\n"); 
#endif 

	ierr = MPI_Type_create_subarray(NDIM, arraygsize, arraysubsize, arraystart,
			MPI_ORDER_C, dataType, &filetype); 
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
	ierr = MPI_File_open(cartcomm, FILENAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, 
			MPI_INFO_NULL, &fh); 
	mpi_error_check(ierr); 
#ifndef NDEBUG   
	printf("mpiWrite ->MPI file open \n"); 
#endif       

	// Set view for this process using datatype 
	ierr = MPI_File_set_view(fh, 0, dataType, filetype, "native",  
			MPI_INFO_NULL); 
	mpi_error_check(ierr); 
#ifndef NDEBUG   
	printf("mpiWrite ->MPI file set view \n"); 
#endif       


	int total_data = 1; 
	for (i = 0; i< NDIM; i++)
	{
		total_data *= arraysubsize[i]; 
	}
	ierr = MPI_File_write_all(fh, iodata, total_data, dataType, &status);   
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


