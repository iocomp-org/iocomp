/** @file hdf5Read.c
 *  @brief Reads data from hdf5 file serially.
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */
#include <H5FDmpi.h>
#include <H5FDmpio.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <hdf5.h>
#include <H5FDmpio.h>
#include <hdf5_hl.h>
#include "../include/iocomp.h"
#define DATASETNAME "/IntArray" 

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/**
 * @file hdf5Read.c
 * @brief Reads data from hdf5 file serially.
 * example taken from https://raw.githubusercontent.com/HDFGroup/hdf5/develop/examples/h5_rdwt.c 
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param iodata_test pointer to array to read data into
 * @param fileName name of file to read data from
 * @return void
 */
void hdf5Read(struct iocomp_params *iocompParams, double* iodata_test, char* fileName)
{
	hid_t  file_id, dataset_id; /* identifiers */
	herr_t status;
	
	/* Open an existing file. */
	file_id = H5Fopen(fileName, H5F_ACC_RDWR, H5P_DEFAULT);

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
#pragma GCC diagnostic pop

