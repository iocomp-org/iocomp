#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <hdf5.h>
#include <H5FDmpio.h>
#include <hdf5_hl.h>
#include <adios2_c.h>
#include <mpi.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h> 
#include <getopt.h>
#include <ftw.h>
#include "bench_headerfiles.h" 
#include "../include/iocomp.h"

void data_position(int *local_size, int *global_size, int *arraystart, int *dims_mpi, int *coords, int rank, int n, int NDIM, double* total_size, int SCALING_FLAG)
{
      if (SCALING_FLAG == 1)
      {
#ifndef NDEBUG
        printf("Strong scaling flag start\n");
        {
          local_size[i] = n / dims_mpi[i] + ((n % dims_mpi[i]) > rank ? 1 : 0);
          global_size[i] = n;
          arraystart[i] = local_size[i] * coords[i] + ((local_size[i] % dims_mpi[i]) < rank ? (local_size[i] % dims_mpi[i]) : rank);
          // printf("RANK %i local_size[%i], global_size[%i], arraystart[%i]: %i %i %i \n", rank, i, i, i, local_size[i], global_size[i], arraystart[i]);
        }
#ifndef NDEBUG
        printf("Strong scaling assign local, global sizes and arraystart \n");
#endif
        /*
        * Initialise iodata with 3D array
        */
        *total_size = 1.0;
        for (i = 0; i < NDIM; i++)
        {
          *total_size *= local_size[i]; // n1 * n2 * n3 ... so on. To calculate the total local size
        }
#ifndef NDEBUG
        printf("Total size assigned %lf \n", *total_size);
#endif
      }

      else if (SCALING_FLAG == 0)
      {
#ifndef NDEBUG
        printf("Weak scaling flag started \n");
#endif
        for (i = 0; i < NDIM; i++)
        {
          local_size[i] = n;
          global_size[i] = local_size[i] * dims_mpi[i];
          arraystart[i] = local_size[i] * coords[i];
        }

#ifndef NDEBUG
        printf("size parameters \n");
#endif

        *total_size = 1.0;
        for (i = 0; i < NDIM; i++)
        {
          *total_size *= local_size[i]; // n1 * n2 * n3 ... so on. To calculate the total local size
        }
#ifndef NDEBUG
        printf("total size \n");
#endif
      }

      /*
      * Assign iodata variables. Depends on position of array in global erray.
      */
} 
