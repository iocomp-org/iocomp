/* example taken from https://raw.githubusercontent.com/HDFGroup/hdf5/develop/examples/h5_rdwt.c */ 
#include <H5FDmpi.h>
#include <H5FDmpio.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <hdf5.h>
#include <H5FDmpio.h>
#include <hdf5_hl.h>
#include "../include/iocomp.h"
#define DATASETNAME "/IntArray" 

/*** READ SERIALLY ***/ 

void hdf5Read(struct iocomp_params *iocompParams, double* iodata_test)
{
	hid_t  file_id, dataset_id; /* identifiers */
	herr_t status;

	/* Open an existing file. */
	file_id = H5Fopen(iocompParams->FILENAMES[iocompParams->ioLibNum], H5F_ACC_RDWR, H5P_DEFAULT);

	/* Open an existing dataset. */
	dataset_id = H5Dopen2(file_id, DATASETNAME, H5P_DEFAULT);

	status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, iodata_test);
	mpi_error_check(status); 

	/* Close the dataset. */
	status = H5Dclose(dataset_id);
	mpi_error_check(status); 

	/* Close the file. */
	status = H5Fclose(file_id);
	mpi_error_check(status); 
}


