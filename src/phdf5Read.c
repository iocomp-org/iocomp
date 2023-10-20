#include <omp.h> 
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <hdf5.h>
#include "../include/iocomp.h"
#define DATASETNAME "IntArray"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

void phdf5Read(double *readData, char* fileName, struct iocomp_params *iocompParams) 
{   

	hid_t       file, dataset;        // handles 
	hid_t       dataspace;						// file data space handle 
	hid_t       memspace;							// file memory space handle 
	herr_t      ierr;									// error handlers 
	hsize_t      count[iocompParams->NDIM];         // size of the hyperslab in the file 
	hsize_t      offset[iocompParams->NDIM];        // hyperslab offset in the file 
	hid_t plist_id, xfer_plist;				// File access templates
	MPI_Info info = MPI_INFO_NULL;
	int          i;

	/* 
	 * offset is the starting point of data from the global array 
	 * count is the number of elements in the local array 
	 */
	for (i = 0; i < iocompParams->NDIM; i++)
	{
		offset[i]		= (hsize_t)iocompParams->arrayStart[i]; 
		count[i]		= (hsize_t)iocompParams->localArray[i];  
	}

	/* 
	 * Open HDF5 file 
	 */
	// Set up file access property list with parallel I/O access
	plist_id = H5Pcreate(H5P_FILE_ACCESS); 
	assert(plist_id!= H5I_INVALID_HID); 

	// Set up parallel access with communicator 	
	ierr = H5Pset_fapl_mpio(plist_id, iocompParams->cartcomm, info);
	assert(ierr!= H5I_INVALID_HID); 

	/*
	 * Open the file collectively 
	 */
	file = H5Fopen(fileName, H5F_ACC_RDONLY, plist_id);
	assert(file!= H5I_INVALID_HID); 

	/* Release file-access template */
	ierr =H5Pclose(plist_id);
	assert(ierr!= H5I_INVALID_HID); 

	/*
	 * Open datasets in the file object 
	 */
	dataset = H5Dopen(file, DATASETNAME, H5P_DEFAULT);
	assert(dataset!= H5I_INVALID_HID); 

	// create a file dataspace independently 
	// stride and block are set to be NULL 
	dataspace = H5Dget_space(dataset);    /* dataspace handle */
	assert(ierr!= H5I_INVALID_HID); 
	
	// select hyperslab in the file 
	ierr = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, NULL, 
			count, NULL);
	assert(ierr!= H5I_INVALID_HID); 

	// create memory dataspace independently 
	memspace = H5Screate_simple(iocompParams->NDIM,count,NULL);   
	assert(memspace!= H5I_INVALID_HID); 

	// set up collective transfer properties list 
	xfer_plist = H5Pcreate (H5P_DATASET_XFER);
	assert(xfer_plist!= H5I_INVALID_HID); 

	ierr=H5Pset_dxpl_mpio(xfer_plist, H5FD_MPIO_COLLECTIVE);
	assert(ierr!= H5I_INVALID_HID); 

	/* read data collectively */
	ierr = H5Dread(dataset, H5T_NATIVE_DOUBLE, memspace, dataspace,
			xfer_plist, readData);					    
	assert(ierr!= H5I_INVALID_HID); 
	if( ierr == H5I_INVALID_HID)
	{
		printf("ierr %i \n", ierr); 
	} 

#ifndef NDEBUG 
	fprintf(iocompParams->debug,"File read data output\n"); 
	for(int i = 0; i < iocompParams->localDataSize; i++)
	{
		fprintf(iocompParams->debug, "%lf, ", readData[i]); 
	} 
#endif 

	/*
	 * Close/release resources.
	 */
	H5Dclose(dataset);
	H5Sclose(dataspace);
	H5Sclose(memspace);
	H5Fclose(file);

} 
#pragma GCC diagnostic pop
