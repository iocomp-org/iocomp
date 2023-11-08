#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <adios2_c.h>
#include <adios2/c/adios2_c_types.h>
#if ADIOS2_USE_MPI
#include <mpi.h>
#endif 
#include "../include/iocomp.h"

void adios2ReadSerially(double* iodata, char* FILENAME, struct iocomp_params *iocompParams)
{
	adios2_error errio; 
	adios2_engine *engine = adios2_open(iocompParams->io, FILENAME, adios2_mode_read);
	assert(engine!=NULL); 

	adios2_variable *var_greeting = adios2_inquire_variable(iocompParams->io, "iodata");
	assert(var_greeting!=NULL);

	errio = adios2_get(engine, var_greeting, iodata, adios2_mode_deferred);
	mpi_error_check(errio); 

	adios2_close(engine);
} 

void adios2Read(double* iodata, char* FILENAME, struct iocomp_params *iocompParams)
{   
	adios2_error errio; 

#ifdef VERBOSE
	printf("adios2Read->adios2Init flag %i \n", iocompParams->adios2Init);
#endif
	if(!iocompParams->adios2Init) // check if declared before so that adios2 variable is not defined again. 
	{
		iocompParams->var_iodata = adios2_define_variable(iocompParams->io, "iodata", adios2_type_double,iocompParams->NDIM,
				iocompParams->globalArray, iocompParams->arrayStart, iocompParams->localArray, adios2_constant_dims_true); 
		iocompParams->adios2Init = 1;  
#ifdef VERBOSE
		printf("adios2Read->variable defined \n");
#endif
	}

	adios2_engine *engine = adios2_open(iocompParams->io, FILENAME, adios2_mode_read);
#ifdef VERBOSE
	printf("adios2Read->engine opened \n");
#endif

//	adios2_step_status status; 
//	errio = adios2_begin_step(engine, adios2_step_mode_read, 0, &status);   
//#ifdef VERBOSE
//	printf("adios2Read->begin step \n");
//#endif

	errio = adios2_get(engine,iocompParams->var_iodata, iodata, adios2_mode_deferred);
	mpi_error_check(errio); 
#ifdef VERBOSE
	printf("adios2Read->reading completed \n");
#endif

//	errio = adios2_end_step(engine);
//	mpi_error_check(errio); 
//#ifdef VERBOSE
//	printf("adios2Read->end step\n");
//#endif	

	errio = adios2_close(engine);
	mpi_error_check(errio); 
#ifdef VERBOSE
	printf("adios2Read->engine closed \n");
#endif
}
