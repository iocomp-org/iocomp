// #include <omp.h> 
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
// #define NDIM 3 
#define SCALING_FLAG 1 


// void check_mpi_error(int ierr, MPI_Comm cartcomm)
// {
//     MPI_Errhandler_set(cartcomm,MPI_ERRORS_RETURN); /* return info about */ 
//     int errclass, resultlen; 
//     char err_buffer[MPI_MAX_ERROR_STRING];
//     if (ierr != MPI_SUCCESS) {
//         MPI_Error_class(ierr,&errclass);
//         if (errclass== MPI_ERR_RANK) {
//         fprintf(stderr,"Invalid rank used in MPI send call\n");
//         MPI_Error_string(ierr,err_buffer,&resultlen);
//         fprintf(stderr,err_buffer);
//         MPI_Finalize();             /* abort*/
//         }
//     }
// }

void mpiiowrite(double* iodata, int*arraysubsize, int* arraygsize, int* arraystart, int NDIM, MPI_Comm cartcomm, char* FILENAME)
{   
    int             i, j, k, initialized, finalized,ierr, size, comm, rank, argc, nprocs, myrank, nints, 
                    count[NDIM],
                    dims[NDIM],
                    coords[NDIM], 
                    periods[NDIM],
                    order = 0, 
                    disp = 0; // number of bytes to be skipped from the start. ex headers. 

#ifndef NDEBUG   
    printf("MPI variables init completed \n"); 
#endif 
    
		// MPI initialisations
    MPI_File        fh; 
    MPI_Status      status;
    MPI_Datatype    filetype, mpi_subarray; 

    // MPI_Errhandler_set(cartcomm,MPI_ERRORS_RETURN); /* return info about
    //   errors */
    MPI_Comm_size(cartcomm, &nprocs);
    MPI_Comm_rank(cartcomm, &myrank);
    MPI_Cart_get(cartcomm, NDIM, dims, periods, coords); 
    ierr = MPI_File_open(cartcomm, FILENAME,
                MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &fh); 
    // check_mpi_error(ierr, cartcomm); 
    MPI_Info info  = MPI_INFO_NULL; 
    
#ifndef NDEBUG   
    printf("MPI cartcomm got ranks \n"); 
    printf("arraygsize for rank %i : %i %i  \n",myrank, arraygsize[0],arraygsize[1]); 
    printf("arraysubsize for rank %i : %i %i \n",myrank,arraysubsize[0],arraysubsize[1]); 
    printf("arraystart for rank %i : %i %i \n",myrank,arraystart[0],arraystart[1]); 
#endif 


#ifndef NDEBUG   
    printf("MPI arraystart and arraygsize completed\n"); 
#endif 
    ierr = MPI_Type_create_subarray(NDIM, arraygsize, arraysubsize, arraystart,
                                    MPI_ORDER_C, MPI_DOUBLE, &filetype); 
    // check_mpi_error(ierr, cartcomm); 

#ifndef NDEBUG   
    printf("MPI create subarray \n"); 
#endif                                   
    ierr = MPI_Type_commit(&filetype); 
    // check_mpi_error(ierr, cartcomm); 
#ifndef NDEBUG   
    printf("MPI type commit \n"); 
#endif       
    // Open file
    ierr = MPI_File_open(cartcomm, FILENAME, MPI_MODE_CREATE | MPI_MODE_WRONLY, 
                     MPI_INFO_NULL, &fh); 
    // check_mpi_error(ierr, cartcomm); 
#ifndef NDEBUG   
    printf("MPI file open \n"); 
#endif       
    // Set view for this process using datatype 
    ierr = MPI_File_set_view(fh, 0, MPI_DOUBLE, filetype, "native",  
                         MPI_INFO_NULL); 
    // check_mpi_error(ierr, cartcomm); 
#ifndef NDEBUG   
    printf("MPI file set view \n"); 
#endif       
    // Remove halo data    
    int total_data = 1; 
    for (i = 0; i< NDIM; i++)
    {
        total_data *= arraysubsize[i]; 
    }
    ierr = MPI_File_write_all(fh, iodata, total_data, MPI_DOUBLE, &status);   
    // check_mpi_error(ierr, cartcomm); 
#ifndef NDEBUG   
    printf("MPI file write all \n"); 
#endif       
    ierr = MPI_File_close(&fh);
    // check_mpi_error(ierr, cartcomm); 
#ifndef NDEBUG   
    printf("MPI file close \n"); 
#endif       
    ierr = MPI_Type_free(&filetype); 
    // check_mpi_error(ierr, cartcomm); 
#ifndef NDEBUG   
    printf("MPI filetype\n"); 
#endif       
}
                           

