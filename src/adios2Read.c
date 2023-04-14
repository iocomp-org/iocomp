#include <adios2_c.h>
#include <adios2/c/adios2_c_types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../include/iocomp.h"

double* adios2Read(struct iocomp_params *iocompParams)
{   
	int											ierr; 	
	float                   times = 0; 
	double* iodata_test = NULL; 
	iodata_test = (double*)malloc(iocompParams->globalDataSize*sizeof(double));

	/*
	 * Since this is rank 0 testing for files, adios2 is declared serially.
	 */ 
	adios2_adios *adios = adios2_init_serial(); 
	adios2_io *io = adios2_declare_io(adios, 
			iocompParams->ADIOS2_IOENGINES[iocompParams->ioLibNum-2]); //IO handler declaration
	adios2_engine *engine = adios2_open(io, iocompParams->FILENAMES[iocompParams->ioLibNum], adios2_mode_read);
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
	mpi_error_check(ierr); 

	ierr = adios2_close(engine);
	mpi_error_check(ierr); 

	engine = NULL; 
	adios2_finalize(adios); 

	return iodata_test; 
}

