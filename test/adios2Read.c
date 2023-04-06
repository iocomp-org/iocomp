#include <adios2_c.h>
#include <adios2/c/adios2_c_types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "test.h"

bool adios2Read(struct test_variables* testParams, int IO)
{   
	int                     i, nprocs, myrank, div, remainder, initial, final, ierr = 10, 
													max_global_size = 1; 
	float                   times = 0; 
	double* iodata_test = NULL; 
	iodata_test = (double*)malloc(testParams->globalDataSize*sizeof(double));

	printf("Data verification started for filename %s \n", testParams->FILENAMES[IO]); 
	/*
	 * Since this is rank 0 testing for files, adios2 is declared serially.
	 */ 
	adios2_adios *adios = adios2_init_serial(); 
	adios2_io *io = adios2_declare_io(adios, testParams->adios2Engine[IO-2]); //IO handler declaration
	adios2_engine *engine = adios2_open(io, testParams->FILENAMES[IO], adios2_mode_read);
	adios2_variable *var_iodata = adios2_inquire_variable(io, "iodata"); 

	/*
	 * start step to initiate data stream 
	 * adios2_get function to insert contents of var_iodata (adios2 variable) into iodata_read
	 * end step to stop data stream
	 */ 
	adios2_step_status status = 0; 
	adios2_begin_step(engine, adios2_step_mode_read, times, &status); 
	adios2_get(engine, var_iodata , iodata_test, adios2_mode_sync);
	ierr = adios2_end_step(engine);

	ierr = adios2_close(engine);
	engine = NULL; 
	adios2_finalize(adios); 

	arrayVerification(testParams, iodata_test); 
	printf("Data verification complete for filename %s \n", testParams->FILENAMES[IO]); 

	free(iodata_test); 
	iodata_test = NULL; 
	return 0; 
}

