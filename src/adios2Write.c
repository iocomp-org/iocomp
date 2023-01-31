#include <adios2_c.h>
#include <adios2/c/adios2_c_types.h>
#include <mpi.h>
#include <hdf5.h>
#include <stdlib.h>
#include <string.h>

// #define FILE        "iodata-c-hdf5.h5"
#define DATASETNAME "/Geometry/geometry/geometry"
// #define NDIM 3 
#define CONSTANT_DIMS 1 
#define min(X,Y) (((X) < (Y)) ? (X) : (Y))
#define config_file "config.xml"

// void reader(adios2_adios *adios, long double *iodata)
// {
//     adios2_io *io = adios2_declare_io(adios, "IODATA-reader");
//     adios2_engine *engine = adios2_open(io, "iodata-c-hdf5.h5", adios2_mode_read);
//     adios2_variable *var_iodata = adios2_inquire_variable(io, "iodata");
//     adios2_get(engine, var_iodata, iodata, adios2_mode_deferred);
//     adios2_close(engine);
// }

void adioswrite(double* iodata, int*arraysubsize, int* arraygsize, int* arraystart, int NDIM, MPI_Comm cartcomm,  char* IO_ENGINE, char* FILENAME)
{   
    // count and offset matrices for individual ranks.  
    int                     i, nprocs, myrank, div, remainder, initial, final; 
    size_t                  shape[NDIM], 
                            count[NDIM], 
                            start[NDIM]; 

    /* 
    * Initialisations 
    */ 
    
    for (i = 0; i < NDIM; i++)
    {
        shape[i] = arraygsize[i]; 
        count[i] = arraysubsize[i]; 
        start[i] = arraystart[i]; 
    }
    MPI_Comm_size(cartcomm, &nprocs);
    MPI_Comm_rank(cartcomm, &myrank);

    /*
    shape[0] is divided between different ranks, the 
    rest of the dimensions are same for all ranks 
    */


    // shape[0] *= nprocs;
    // start[0] = myrank * n1;

    //adios2_adios *adios = adios2_init_config(config_file, 1); // cartcomm); // if using ADIOS2 MPI, need to include debugger. 
    //adios2_adios *adios = adios2_init_config(config_file, cartcomm, 1); // cartcomm); // if using ADIOS2 MPI, need to include debugger. 
    adios2_adios *adios = adios2_init_config_mpi(config_file, cartcomm); // cartcomm); // if using ADIOS2 MPI, need to include debugger. 
    adios2_io *io = adios2_declare_io(adios, IO_ENGINE); //IO handler declaration
    
    /*
    * constant_dims true variables constant, false variables can change after definition. For every rank this should be true.
    * shape is global dimension
    * start is local offset 
    * count is local dimension
    */

    adios2_variable *var_iodata = adios2_define_variable(io, "iodata", adios2_type_double, NDIM,
                                                         shape, start, count, adios2_constant_dims_true); 

    adios2_engine *engine = adios2_open(io, FILENAME, adios2_mode_write);
    adios2_step_status status; 
    adios2_begin_step(engine, adios2_step_mode_update, 10.0, &status);   
    adios2_put(engine, var_iodata, iodata, adios2_mode_deferred);
    adios2_end_step(engine);
    adios2_close(engine);
    adios2_finalize(adios); 
}
