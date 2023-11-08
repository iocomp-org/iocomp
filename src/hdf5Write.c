#include <omp.h> 
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <hdf5.h>
#include <H5FDmpio.h>
#include <H5FDmpi.h> 
#include <hdf5_hl.h> 
#include "../include/iocomp.h"

void phdf5write(double* iodata, struct iocomp_params *iocompParams, int windowNum)
{   
    // Variable initialisation
    char            dsetname[100] = "IntArray";
    // HDF5 initialisations
    hid_t           file_id, // file identifier
                    dset_id, // dataset identifier 
                    filespace, // dataspace identifier in file 
                    memspace, // dataspace identifier in memory
                    plist_id;  // property list identifier 
    hsize_t         count[iocompParams->NDIM], 
                    offset[iocompParams->NDIM],
                    dimsf[iocompParams->NDIM];   // specifies the dimensions of dataset, dimsf[0] number of rows, dimsf[1] number of columns, dimsf[2] so on..

#ifdef VERBOSE
    fprintf(iocompParams->debug,"inits \n"); 
#endif

    herr_t status = H5open();
		mpi_error_check(status); 
#ifdef VERBOSE
    fprintf(iocompParams->debug,"After H5open\n"); 
#endif
    MPI_Info info  = MPI_INFO_NULL; 

    /* 
    * Initialisations from arrays passed in the function 
    */ 

    for (int i = 0; i < iocompParams->NDIM; i++)
    {
        dimsf[i] = iocompParams->globalArray[i]; 
        count[i] = iocompParams->localArray[i]; 
        offset[i]= iocompParams->arrayStart[i]; 
#ifdef VERBOSE
        assert(dimsf[i] > 0); 
        assert(count[i] > 0); 
#endif
    }
		
#ifdef VERBOSE 
	fprintf(iocompParams->debug, "HDF5 Write-> array specifiers \n"); 
	fprintf(iocompParams->debug,"dimsf"); 
	for(int i = 0; i < iocompParams->NDIM; i++)
	{
		fprintf(iocompParams->debug, "[%llu]", dimsf[i]); 
	} 
	fprintf(iocompParams->debug, "\n"); 
	fprintf(iocompParams->debug,"count"); 
	for(int i = 0; i < iocompParams->NDIM; i++)
	{
		fprintf(iocompParams->debug, "[%llu]", count[i]); 
	} 
	fprintf(iocompParams->debug, "\n"); 
	fprintf(iocompParams->debug,"offset"); 
	for(int i = 0; i < iocompParams->NDIM; i++)
	{
		fprintf(iocompParams->debug, "[%llu]", offset[i]); 
	} 
	fprintf(iocompParams->debug, "\n"); 
#endif 

	/* 
	 * Set up file access property list with parallel I/O access
	 */
    plist_id = H5Pcreate(H5P_FILE_ACCESS); 
    H5Pset_fapl_mpio(plist_id, iocompParams->cartcomm, info);
#ifdef VERBOSE
    fprintf(iocompParams->debug,"file access \n"); 
#endif

    /*
     * Create a new file collectively and release property list identifier.
     */
		 
    file_id = H5Fcreate(iocompParams->writeFile[windowNum], H5F_ACC_TRUNC, H5P_DEFAULT, plist_id);

#ifdef VERBOSE
    fprintf(iocompParams->debug,"property list \n"); 
#endif

    /*
     * Create the dataspace for the dataset.
     */
    filespace = H5Screate_simple(iocompParams->NDIM, dimsf, NULL); 
    dset_id = H5Dcreate(file_id, dsetname, H5T_NATIVE_DOUBLE, filespace, 
            H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT); 
    H5Sclose(filespace);

    /* 
     * Each process defines dataset in memory and writes it to the hyperslab
     * in the file.
     */
    memspace = H5Screate_simple(iocompParams->NDIM, count, NULL); 
    
    /*
     * Select hyperslab in the file.
     */
    filespace = H5Dget_space(dset_id); // makes a copy of the dataspace FOR the dataset dset_id
    H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, count, NULL); 

    /*
     * Create property list for collective dataset write.
     */
    plist_id = H5Pcreate(H5P_DATASET_XFER); // sets data transfer mode.
    H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE); // sets data transfer mode.
		
    status = H5Dwrite (dset_id, H5T_NATIVE_DOUBLE, memspace, filespace, 
            plist_id, iodata);
#ifdef VERBOSE
    fprintf(iocompParams->debug,"property list collective data writes \n"); 
#endif

    // Free resources
    status = H5Sclose (memspace);
    status = H5Sclose (filespace);
    status = H5Dclose (dset_id);    
    status = H5Pclose (plist_id); 
    status = H5Fclose (file_id);

}
