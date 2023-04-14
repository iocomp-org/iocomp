#include <adios2_c.h>
#include <adios2/c/adios2_c_types.h>
#include <mpi.h>
#include <hdf5.h>
#include <stdlib.h>
#include <string.h>
#include "../include/iocomp.h"

void adioswrite(double* iodata, struct iocomp_params *iocompParams)
{   
	/* 
	 * Assert tests to check for negative values 
	 */ 
	for (int i = 0; i <iocompParams->NDIM; i++)
	{
		assert(iocompParams->localArray[i]  > 0); 
		assert(iocompParams->globalArray[i] > 0); 
	}
	adios2_error errio; 

#ifndef NDEBUG
	printf("adios2Write->adios2Init flag %i \n", iocompParams->adios2Init);
#endif
	if(!iocompParams->adios2Init) // check if declared before so that adios2 variable is not defined again. 
	{
	iocompParams->var_iodata = adios2_define_variable(iocompParams->io, "iodata", adios2_type_double,iocompParams->NDIM,
			iocompParams->globalArray, iocompParams->arrayStart, iocompParams->localArray, adios2_constant_dims_true); 
		iocompParams->adios2Init = 1;  
#ifndef NDEBUG
	printf("adios2Write->variable defined \n");
#endif
	}

	adios2_engine *engine = adios2_open(iocompParams->io, iocompParams->FILENAMES[iocompParams->ioLibNum], adios2_mode_write);
#ifndef NDEBUG
	printf("adios2Write->engine opened \n");
#endif

	adios2_step_status status; 
	errio = adios2_begin_step(engine, adios2_step_mode_update, 10.0, &status);   
#ifndef NDEBUG
	printf("adios2Write->begin step \n");
#endif

	errio = adios2_put(engine,iocompParams->var_iodata, iodata, adios2_mode_deferred);
	mpi_error_check(errio); 
#ifndef NDEBUG
	printf("adios2Write->writing completed \n");
#endif

	errio = adios2_end_step(engine);
	mpi_error_check(errio); 
#ifndef NDEBUG
	printf("adios2Write->end step\n");
#endif	

	errio = adios2_flush(engine); 
	mpi_error_check(errio); 
#ifndef NDEBUG
	printf("adios2Write->flush I/O engine\n");
#endif	

	errio = adios2_close(engine);
	mpi_error_check(errio); 
#ifndef NDEBUG
	printf("adios2Write->engine closed \n");
#endif
}
