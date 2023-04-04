#include <adios2_c.h>
#include <adios2/c/adios2_c_types.h>
#include <mpi.h>
#include <hdf5.h>
#include <stdlib.h>
#include <string.h>
#include "../include/iocomp.h"


void adioswrite(double* iodata, size_t* localArray,	size_t* globalArray, size_t* arrayStart, int NDIM, MPI_Comm cartcomm, char *IO_ENGINE, char* FILENAME)
{   
    int                     i, nprocs, myrank, div, remainder, initial, final; 

    /* 
    * Assert tests to check for negative values 
    */ 
    for (i = 0; i < NDIM; i++)
    {
			assert(localArray[i]  > 0); 
			assert(globalArray[i] > 0); 
    }

    MPI_Comm_size(cartcomm, &nprocs);
    MPI_Comm_rank(cartcomm, &myrank);

    //adios2_adios *adios = adios2_init_config_mpi(config_file, cartcomm); // cartcomm); // if using ADIOS2 MPI, need to include debugger. 
		
    adios2_io *io = adios2_declare_io(iocompParams->adios, IO_ENGINE); //IO handler declaration
    // adios2_io *io = adios2_at_io(adios, IO_ENGINE); //IO handler find
    
    adios2_variable *var_iodata = adios2_define_variable(io, "iodata", adios2_type_double, NDIM,
                                                         globalArray, arrayStart, localArray, adios2_constant_dims_true); 

    adios2_engine *engine = adios2_open(io, FILENAME, adios2_mode_write);
    adios2_step_status status; 
    adios2_begin_step(engine, adios2_step_mode_update, 10.0, &status);   
    adios2_put(engine, var_iodata, iodata, adios2_mode_deferred);
    adios2_end_step(engine);
    adios2_close(engine);
    adios2_finalize(adios); 
}
