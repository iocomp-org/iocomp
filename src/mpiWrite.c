#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include "../include/iocomp.h"

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

	// Initialise int arrays for MPIIO 
	int localArray[iocompParams->NDIM]; 
	int globalArray[iocompParams->NDIM]; 
	int arrayStart[iocompParams->NDIM]; 
	int total_data = (int)iocompParams->localDataSize; 
#ifdef VERBOSE   
	fprintf(iocompParams->debug,"mpiWrite -> declare arrays in integers\n"); 
#endif 
	
	/* 
	 * If 1D chosen, use File write all at to use offsets, which are size_t
	 * compatible to avoid int 32 overflows.
	 * Otherwise stick to array decomposition. 
	 */ 
	if(iocompParams->NDIM == 1)
	{
		ierr = MPI_File_open(iocompParams->cartcomm, iocompParams->writeFile[windowNum],
				MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh); 
		mpi_error_check(ierr); 
		openTime = MPI_Wtime() - openTime; 
#ifdef VERBOSE   
		fprintf(iocompParams->debug,"mpiWrite -> MPI file open\n"); 
#endif 

		MPI_Offset offset = iocompParams->arrayStart[0];
#ifdef VERBOSE   
		fprintf(iocompParams->debug,"mpiWrite -> MPI offset assigned\n"); 
#endif       

		ierr = MPI_File_write_at_all(fh, offset*sizeof(double), iodata, total_data, MPI_DOUBLE, &status);   
		mpi_error_check(ierr); 
#ifdef VERBOSE   
		fprintf(iocompParams->debug,"mpiWrite -> MPI file write at all\n"); 
#endif       

		ierr = MPI_File_close(&fh);
		mpi_error_check(ierr); 
#ifdef VERBOSE   
		fprintf(iocompParams->debug,"mpiWrite -> MPI file close \n"); 
#endif       
	} 
	else 
	{
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
		// Set view for this process using datatype 
		ierr = MPI_File_set_view(fh, 0, MPI_DOUBLE, filetype, "native",  MPI_INFO_NULL); 
		mpi_error_check(ierr); 
#ifdef VERBOSE   
		fprintf(iocompParams->debug,"mpiWrite -> MPI file set view \n"); 
#endif       
		ierr = MPI_File_open(iocompParams->cartcomm, iocompParams->writeFile[windowNum],
				MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh); 
		mpi_error_check(ierr); 
		openTime = MPI_Wtime() - openTime; 

#ifdef VERBOSE   
		fprintf(iocompParams->debug,"mpiWrite -> MPI file open\n"); 
#endif 
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
		fprintf(iocompParams->debug,"mpiWrite -> MPI filetype free\n"); 
#endif       
	}
}


