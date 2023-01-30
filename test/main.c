#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "getopt.h"
#include "iocomp.h"
#define NDIM 2 
static int verbose_flag;
static int HT_flag; 

int main(int argc, char** argv)
{

  int ierr;
  ierr = MPI_Init(&argc, &argv);  
  mpi_error_check(ierr); 

	HT_flag = 1; 

  // check for HT flag 
  if (HT_flag)
    puts ("HT flag is set to on");
  else 
    puts ("HT flag is switched off"); 

  MPI_Comm comm; 
  MPI_Comm_dup(MPI_COMM_WORLD,&comm); 
  int rank; 
  MPI_Comm_rank(comm, &rank); 
  // data parameters definitions 

  int localArraySize[NDIM] = {4,4}; 
	size_t localDataSize = 1; 
	for(int i = 0; i < NDIM; i++)
	{
		localDataSize *= localArraySize[i]; 
	}

  struct iocomp_params iocompParams; 

  iocompInit(&iocompParams, comm,  NDIM, localArraySize, HT_flag); 
#ifndef NDEBUG
  printf("After intercommInit\n"); 
#endif

  double* data = NULL; // initialise data pointer  
  data = (double*)malloc(localDataSize*sizeof(double)); // one rank only sends to one rank

	// populate values of test array 
	for(int j=0; j < localDataSize; j++)
	{
		data[j] = (double)j*2; 
	}

	MPI_Request request; 
	dataSend(data,&iocompParams, &request); // send data off using dataSend
	arrayParamsInit(&iocompParams,comm,NDIM,localArraySize);

	ioServerInitialise(&iocompParams); 

	dataWait(&iocompParams,&request);

  stopSend(&iocompParams); 

  MPI_Finalize(); 
#ifndef NDEBUG
  printf("MPI finalize\n"); 
#endif   

  // remove file 
  // if(rank == 0)
  // {
  //   int ret; 
  //   ret = remove("mpiio.dat");
  //   if(ret != 0) {
  //     printf("Error: unable to delete the file");
  //   }
  // } 
#ifndef NDEBUG
  printf("Deleted file\n"); 
#endif   
  free(data); 
  data = NULL; 
  return 0; 
} 

