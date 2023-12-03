#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"
#include <stdint.h>
#include <limits.h>

// what is size_t in terms of MPI data types 
// source: https://stackoverflow.com/questions/40807833/sending-size-t-type-data-with-mpi
#if SIZE_MAX == UCHAR_MAX
#define my_MPI_SIZE_T MPI_UNSIGNED_CHAR
#elif SIZE_MAX == USHRT_MAX
#define my_MPI_SIZE_T MPI_UNSIGNED_SHORT
#elif SIZE_MAX == UINT_MAX
#define my_MPI_SIZE_T MPI_UNSIGNED
#elif SIZE_MAX == ULONG_MAX
#define my_MPI_SIZE_T MPI_UNSIGNED_LONG
#elif SIZE_MAX == ULLONG_MAX
#define my_MPI_SIZE_T MPI_UNSIGNED_LONG_LONG
#else
#error "SIZE_t undefined for MPI operations."
#endif

#define HIGH_LOW 1
#define FIXED_IO_RANK 0

/*
 * Initialises the iocomp_params struct 
 */
void arrayParamsInit(struct iocomp_params *iocompParams, MPI_Comm comm )
{


  int ioSize,ioRank, i; 
  MPI_Comm_size(comm, &ioSize);
  MPI_Comm_rank(comm, &ioRank);

  // iocompParams->dataType = MPI_DOUBLE; // data type of sent and recvd data 
#ifdef VERBOSE
  fprintf(iocompParams->debug,"arrayParamsInit -> local, global size initialised \n"); 
#endif

  /*
   * Obtain localSize from 1D into 2D to avoid int32 overflows.
   * localSize is decomposed into NDIM i.e. 2D array for now. 
   * works based on closest sq root. 
   * if not multiple then goes down the range checking for numbers below the sq
   * root and checking if thats a factorial of the local size or not. 
   */ 
  float power = (float)1/iocompParams->NDIM; 
  int root; 
  size_t dim[2] = {0,0}; // setting this to be 2 dimensions   
  root = (int)pow(iocompParams->localDataSize,power);
#ifdef VERBOSE
  fprintf(iocompParams->debug,"arrayParamsInit -> local data size %ld \n", iocompParams->localDataSize); 
#endif

  // if its a perfect square 
  if(pow(root,iocompParams->NDIM) == iocompParams->localDataSize)
  {
    dim[0] = root; 
    dim[1] = root; 
  }

  // if closest sq root is a perfect factorial 
  else if(iocompParams->localDataSize%root == 0) 
  {
    dim[0] = root; 
    dim[1] = iocompParams->localDataSize/root; 
  }
  // or if closest sq root is not a perfect factorial 
  // then reduce the numbers and check for the closest factorials 
  else if(iocompParams->localDataSize%root != 0)
  {
    for(int i = 1; i < root; i++)
    {
      if(iocompParams->localDataSize%(root-i) == 0) 
      {
        dim[0] = root - i; 
        dim[1] = iocompParams->localDataSize/(root-i); 
        break; 
      }
    }
  }

  // final check if the array dimensions multiply to give the local data size 
  // assert( (iocompParams->localArray[0]*iocompParams->localArray[1]) == iocompParams->localDataSize);

  /* 
   * get count and global data size, using data from other processes by
   * allgather 
   */
  // set data type constant.
  //unsigned long int localArray[iocompParams->NDIM]; 
  // TODO: use malloc etc for NDIM offsets
  unsigned long int recv_localArray1D[ioSize]; 
  unsigned long int recv_localArray2D[ioSize];

  for(i = 0; i < ioSize; i++)
  {
    recv_localArray1D[i] = 0; 
    recv_localArray2D[i] = 0; 
  }

  recv_localArray1D[ioRank] = dim[0]; 
  recv_localArray2D[ioRank] = dim[1]; 

  // unsigned long int all_localArray[iocompParams->NDIM][ioSize]; // cos need to get data from all arrays. 
  // unsigned long int globalArray[iocompParams->NDIM]; 

  //for (i = 0; i < iocompParams->NDIM; i++)
  //{
  //  iocompParams->localArray[i] = dim[i]; 
  //  localArray[i] = (unsigned long int)dim[i]; 
  //}

  // Go over each dimension and find its offset point and global dimensions. 
  //for (i = 0; i < iocompParams->NDIM; i++)
  //{
  //  // Sum of all local sizes will give global array 
  //  MPI_Allreduce(&localArray[i], &globalArray[i], 1, MPI_UNSIGNED_LONG, MPI_SUM, iocompParams->ioServerComm); 
  //} 
  
  // Get offset for array using sum of every local dimension size before current rank. Do this over NDIM
  // TODO: implement NDIM loop 
  // Distribute localArray split into NDIM dimensions.
  //MPI_Allgather(&localArray[0], 1, MPI_UNSIGNED_LONG, &recv_localArray1D, 1, MPI_UNSIGNED_LONG, iocompParams->ioServerComm); 
  //MPI_Allgather(&localArray[1], 1, MPI_UNSIGNED_LONG, &recv_localArray2D, 1, MPI_UNSIGNED_LONG, iocompParams->ioServerComm); 
  
  //  get global size; sum of all data sizes subtract by 1 to get C arrays.
  MPI_Allreduce(MPI_IN_PLACE, recv_localArray1D, ioSize, MPI_UNSIGNED_LONG, MPI_SUM,iocompParams->ioServerComm); 
  MPI_Allreduce(MPI_IN_PLACE, recv_localArray2D, ioSize, MPI_UNSIGNED_LONG, MPI_SUM,iocompParams->ioServerComm); 
  
  //// get recv local array 1 and 2d. 
  //for(i = 0; i < ioSize; i++)
  //{
  //  printf("[%i](%lu, %lu) ", i, recv_localArray1D[i], recv_localArray2D[i]); 
  //}
  //printf("\n"); 
  // need to initialise as array start will be iteratively summed.
  for (i = 0; i < iocompParams->NDIM; i++)
  {
    iocompParams->arrayStart[i] = 0; 
    iocompParams->globalArray[i] = 0; 
    iocompParams->localArray[i] = 0; 
  } 

  // after all gather, get localArray dimensions for each rank which counts to elements sent to
  // ranks before it and that becomes offset point at that rank. 
  // TODO: implement NDIM loop 
  for(int j = 0; j < ioRank; j++)
  {
    iocompParams->arrayStart[0] += (size_t)recv_localArray1D[j]; 
    iocompParams->arrayStart[1] += (size_t)recv_localArray2D[j]; 
  } 
  // get global size from sum of local sizes for each rank. 
  for(i = 0; i < ioSize; i++)
  {
    iocompParams->globalArray[0] += (size_t)recv_localArray1D[i];  
    iocompParams->globalArray[1] += (size_t)recv_localArray2D[i];  
  }
  iocompParams->localArray[0] = (size_t)dim[0]; 
  iocompParams->localArray[1] = (size_t)dim[1]; 

//  printf("Rank %i - globalArray:[%lu,%lu], localArray:	[%lu,%lu], arrayStart:	[%lu,%lu] \n",
//    ioRank, globalArray[0],globalArray[1],localArray[0],localArray[1],arrayStart[0],arrayStart[1] ); 

  // assign all values to iocomp structure
  //for (i = 0; i < iocompParams->NDIM; i++)
  //{
  //  iocompParams->globalArray[i] = (size_t)globalArray[i]; 
  //  iocompParams->localArray[i] = (size_t)localArray[i]; 
  //  iocompParams->arrayStart[i] = (size_t)arrayStart[i]; 	 
  //} 
  printf("Rank %i - globalArray:[%lu,%lu], localArray:	[%lu,%lu], arrayStart:	[%lu,%lu] \n",
      ioRank, iocompParams->globalArray[0], iocompParams->globalArray[1],iocompParams->localArray[0],
      iocompParams->localArray[1],iocompParams->arrayStart[0],iocompParams->arrayStart[1]); 

#ifdef VERBOSE   
  fprintf(iocompParams->debug,"arrayParamsInit-> globalArray:[%li,%li] \n",iocompParams->globalArray[0], iocompParams->globalArray[1] ); 
  fprintf(iocompParams->debug,"arrayParamsInit-> localArray:[%li,%li] \n", iocompParams->localArray[0],  iocompParams->localArray[1] ); 
  // fprintf(iocompParams->debug,"arrayParamsInit-> startArray:[%li,%li] \n", iocompParams->arrayStart[0],  iocompParams->arrayStart[1] ); 
#endif 

  /*
   * Array local, global data sizes initialise 
   */ 
  iocompParams->globalDataSize = iocompParams->localDataSize * ioSize; 

#ifdef VERBOSE
  fprintf(iocompParams->debug,"arrayParamsInit -> size definitions, localDataSize %li, globalDataSize %li\n", iocompParams->localDataSize, iocompParams->globalDataSize); 
#endif


#ifdef VERBOSE
  fprintf(iocompParams->debug,"arrayParamsInit -> arrayStart initialised ioRank = %i\n",ioRank);
#endif

} 
