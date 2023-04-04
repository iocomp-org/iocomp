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
		assert(localArray[i]  > 0); 
		assert(globalArray[i] > 0); 
	}

	if(!iocompParams->previousInit) // check if declared before so that adios2 variable is not defined again. 
	{
		iocompParams->var_iodata = adios2_define_variable(iocompParams->io, "iodata", adios2_type_double,iocompParams->NDIM,
				iocompParams->globalArray, iocompParams->arrayStart, iocompParams->localArray, adios2_constant_dims_true); 
		iocompParams->previousInit = 1;  
	}
#ifndef NDEBUG
	printf("adios2Write->variable defined \n");
#endif

	adios2_engine *engine = adios2_open(iocompParams->io, iocompParams->FILENAMES[iocompParams->ioLibNum], adios2_mode_write);
#ifndef NDEBUG
	printf("adios2Write->engine opened \n");
#endif

	adios2_step_status status; 
	adios2_begin_step(engine, adios2_step_mode_update, 10.0, &status);   
#ifndef NDEBUG
	printf("adios2Write->begin step \n");
#endif

	adios2_put(engine,iocompParams->var_iodata, iodata, adios2_mode_deferred);
#ifndef NDEBUG
	printf("adios2Write->writing completed \n");
#endif

	adios2_end_step(engine);
#ifndef NDEBUG
	printf("adios2Write->end step\n");
#endif

	adios2_close(engine);
#ifndef NDEBUG
	printf("adios2Write->engine closed \n");
#endif
}
