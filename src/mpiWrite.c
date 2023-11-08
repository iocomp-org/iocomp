/** @file mpiWrite.c
 *  @brief Contains functions for writing data using MPI.
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "../include/iocomp.h"

/**
 * @file mpiWrite.c
 * @brief Contains functions for writing data using MPI.
 * Contains functions for writing data using MPI in parallel. 
 * Window number is used to select the correct file to write to in the array of file names
 * for the shared method, but for the non shared windows, 0 is passed by default.  
 * @param iodata pointer to array to write data from
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param windowNum window number
 * @return void
 */
void mpiiowrite(double* iodata, struct iocomp_params *iocompParams, int windowNum)
{   
	int			i, ierr, nprocs, ioRank; 

	int			dims[iocompParams->NDIM],
					coords[iocompParams->NDIM], 
					periods[iocompParams->NDIM]; 
	
	ierr = MPI_Comm_size(iocompParams->cartcomm, &nprocs);
	mpi_error_check(ierr); 
	ierr = MPI_Comm_rank(iocompParams->ioServerComm, &ioRank);
	mpi_error_check(ierr); 

	// initialise dims, periods and coords
	for(i = 0; i < iocompParams->NDIM; i++)
	{
		dims[i] = 0; 
		coords[i] = 0; 
		periods[i] = 0; 
	}
#ifdef VERBOSE  
	fprintf(iocompParams->debug,"mpiWrite -> MPI variables init completed with NDIM %i\n", iocompParams->NDIM); 
#endif 

	// MPI initialisations
	MPI_File        fh; 
	MPI_Status      status;
	MPI_Datatype    filetype; 

#ifdef VERBOSE   
	fprintf(iocompParams->debug,"mpiWrite -> MPI rank %i and size %i \n", ioRank, nprocs); 
#endif 
	ierr = MPI_Cart_get(iocompParams->cartcomm, iocompParams->NDIM, dims, periods, coords); 
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"mpiWrite -> MPI cartget \n"); 
#endif 
	double openTime = MPI_Wtime(); 
	ierr = MPI_File_open(iocompParams->cartcomm, iocompParams->writeFile[windowNum],
			MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh); 
	mpi_error_check(ierr); 
	openTime = MPI_Wtime() - openTime; 

#ifdef VERBOSE   
	fprintf(iocompParams->debug,"mpiWrite -> MPI file open\n"); 
#endif 

	// Initialise int arrays for MPIIO 
	int localArray[iocompParams->NDIM]; 
	int globalArray[iocompParams->NDIM]; 
	int arrayStart[iocompParams->NDIM]; 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"mpiWrite -> declare arrays in integers\n"); 
#endif 
	
	for(i = 0; i < iocompParams->NDIM; i++)
	{
		localArray[i] =	 (int)iocompParams->localArray[i]; 
		globalArray[i] = (int)iocompParams->globalArray[i]; 
		arrayStart[i] =  (int)iocompParams->arrayStart[i]; 
		assert(localArray[i] > 0); 
		assert(globalArray[i] > 0); 
	}

#ifdef VERBOSE   
	fprintf(iocompParams->debug,"mpiWrite -> globalArray:[%i,%i] \n",globalArray[0], globalArray[1] ); 
	fprintf(iocompParams->debug,"mpiWrite -> localArray:[%i,%i] \n",localArray[0], localArray[1] ); 
	fprintf(iocompParams->debug,"mpiWrite -> startArray:[%i,%i] \n",arrayStart[0], arrayStart[1] ); 
#endif 

	ierr = MPI_Type_create_subarray(iocompParams->NDIM, globalArray, localArray, arrayStart,
			MPI_ORDER_C, MPI_DOUBLE, &filetype); 
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"mpiWrite -> MPI create subarray \n"); 
#endif                                   

	ierr = MPI_Type_commit(&filetype); 
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"mpiWrite -> MPI type commit \n"); 
#endif       

	// Open file
	ierr = MPI_File_open(iocompParams->cartcomm, iocompParams->writeFile[windowNum], 
					MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh); 
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"mpiWrite -> MPI file open \n"); 
#endif       

	// Set view for this process using datatype 
	ierr = MPI_File_set_view(fh, 0, MPI_DOUBLE, filetype, "native",  MPI_INFO_NULL); 
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"mpiWrite -> MPI file set view \n"); 
#endif       

	int total_data = 1; 
	for (i = 0; i< iocompParams->NDIM; i++)
	{
		total_data *= localArray[i]; 
	}
	ierr = MPI_File_write_all(fh, iodata, total_data, MPI_DOUBLE, &status);   
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"mpiWrite -> MPI file write all \n"); 
#endif       

	ierr = MPI_File_close(&fh);
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"mpiWrite -> MPI file close \n"); 
#endif       

	ierr = MPI_Type_free(&filetype); 
	mpi_error_check(ierr); 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"mpiWrite -> MPI filetype\n"); 
#endif       
}


