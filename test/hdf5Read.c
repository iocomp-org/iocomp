/* example taken from https://raw.githubusercontent.com/HDFGroup/hdf5/develop/examples/h5_rdwt.c */ 
#include <H5FDmpi.h>
#include <H5FDmpio.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <hdf5.h>
#include <H5FDmpio.h>
#include <hdf5_hl.h>
#include "test.h"
#define DATASETNAME "/IntArray" 

/*** READ SERIALLY ***/ 

bool hdf5Read(struct test_variables* testParams)
{
	printf("Testing file %s ...\n", testParams->FILENAMES[1]); 
	int i, j, max_global_size = 1; 

	hid_t  file_id, dataset_id; /* identifiers */
	herr_t status;

	double* iodata_test = NULL; 
	iodata_test = (double*)malloc(testParams->globalDataSize*sizeof(double));

	/* Open an existing file. */
	file_id = H5Fopen(testParams->FILENAMES[1], H5F_ACC_RDWR, H5P_DEFAULT);

	/* Open an existing dataset. */
	dataset_id = H5Dopen2(file_id, DATASETNAME, H5P_DEFAULT);

	status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, iodata_test);
	printf("Testing for array values ... \n"); 
	for ( int i = 0; i < testParams->globalDataSize; i++)
	{
		if(iodata_test[i]!=1.0)
		{
			printf("Error: value at index %i not matching. Exiting \n", i); 
			exit(1); 
		}
	}
	printf("Testing for array values passed \n"); 

	/* Close the dataset. */
	status = H5Dclose(dataset_id);

	/* Close the file. */
	status = H5Fclose(file_id);

	// free iodata_read
	free(iodata_test); 
	iodata_test = NULL; 
	printf("Testing from file %s complete\n", testParams->FILENAMES[1]); 
	return 0; 
}


