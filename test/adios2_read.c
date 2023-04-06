#include <adios2_c.h>
#include <adios2/c/adios2_c_types.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void adios2read(double* iodata, int* arraygsize, int NDIM, char* IO_ENGINE, char* FILENAME) 
{   
    int                     i, nprocs, myrank, div, remainder, initial, final, ierr = 10, 
                            max_global_size = 1; 
    size_t                  ndims, shape[NDIM];  
    float                   times = 0; 
    for (i = 0; i < NDIM; i++)
    {
        max_global_size *= arraygsize[i];      
    }
    double* iodata_read = NULL; 
    iodata_read = (double*)malloc(max_global_size * sizeof(double) + 2); 

    /*
    * Since this is rank 0 testing for files, adios2 is declared serially.
    */ 
    adios2_adios *adios = adios2_init_serial(); 
    adios2_io *io = adios2_declare_io(adios, IO_ENGINE); //IO handler declaration
    adios2_engine *engine = adios2_open(io, FILENAME, adios2_mode_read);
    adios2_variable *var_iodata = adios2_inquire_variable(io, "iodata"); 

    /*
    * 1st set of tests, to check if the shape and dimensions of variable matches those of iodata. 
    */ 
    adios2_variable_ndims(&ndims, var_iodata);
    if (ndims != NDIM)
    {
        printf("Number of dimensions is not matching. Data verification failed \n"); 
        exit(1); 
    }
    adios2_variable_shape(shape, var_iodata); 
    for (i = 0; i < NDIM; i++)
    {
        if (shape[i] != arraygsize[i])
        {
            printf("Global shape of array is not matching. Data verification failed \n"); 
            exit(1); 
        }
    }

    /*
    * start step to initiate data stream 
    * adios2_get function to insert contents of var_iodata (adios2 variable) into iodata_read
    * end step to stop data stream
    */ 
    adios2_step_status status = 0; 
    adios2_begin_step(engine, adios2_step_mode_read, times, &status); 
    adios2_get(engine, var_iodata , iodata_read, adios2_mode_sync);
    ierr = adios2_end_step(engine);

    ierr = adios2_close(engine);
    engine = NULL; 
    adios2_finalize(adios); 
    
    /*
    * Data checked to see if it is sequential
    */ 
    for (i = 1; i < max_global_size - 1; i++)
    {
        if (iodata_read[i] != iodata_read[i+1] - 1 || iodata_read[i] != iodata_read[i-1] + 1) // checking if the numbers are sequential
        {
            printf("array not matching, for filename %s and element %lf. Exiting job. \n", FILENAME, iodata_read[i]); 
            exit(1); 
        } 
    }
    printf("Data verification complete for filename %s \n", FILENAME); 
    free(iodata_read); 
    iodata_read = NULL; 
}

