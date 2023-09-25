#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"
#include "getopt.h"

int main(int argc, char** argv)
{

  int ierr;
  ierr = MPI_Init(&argc, &argv);  
  mpi_error_check(ierr); 
	
	/* 
	 * initialise structs 
	 */ 
	struct stream_params streamParams; 
  struct iocomp_params iocompParams; 

  /*
   * Command line arguments using getoptlong 
	 * HT option switches on comm splitting 
	 * size option can set size of array during runtime 
   */ 
	
	/*	
	 * initialise stream param structs using command line parameters
	 */ 
	commandLineArgs(&streamParams, argc, argv); 

  MPI_Comm comm = MPI_COMM_WORLD; 
	int rank; 
  MPI_Comm_rank(comm, &rank); 
  // data parameters definitions 
		  
  if(!rank){
    // check for HT flag 
    if (streamParams.HT_flag)
		{
      puts ("HT flag is set to on");
			printf("size of array %i x %i IO num %i \n", streamParams.nx, streamParams.ny, streamParams.io); 
		} 
    else if (streamParams.sharedFlag)
		{
      puts ("Shared flag is set to on");
			printf("size of array %i x %i IO num %i \n", streamParams.nx, streamParams.ny, streamParams.io); 
		} 
    else 
		{
      puts ("HT flag is switched off"); 
			printf("size of array %i x %i IO num %i \n", streamParams.nx, streamParams.ny, streamParams.io); 
		} 
  } 


	/*
	 * iocomp - iocompInit initialises the ioServer 
	 * and initialises the compute comm 
	 */ 
  MPI_Comm computeComm = iocompInit(&iocompParams,comm, streamParams.HT_flag, streamParams.io, NODESIZE, streamParams.sharedFlag); 
#ifndef NDEBUG
  printf("stream->After intercommInit\n"); 
#endif

	
	/*
	 * initialises the local array sizes 
	 * multiplying by size1 x size2
	 */ 
	streamParams.localDataSize = streamParams.nx*streamParams.ny;  
#ifndef NDEBUG
  printf("stream-> localdatasize initialised with %li \n", streamParams.localDataSize); 
#endif

  computeStep(&iocompParams, &streamParams, computeComm); // do compute 
#ifndef NDEBUG
  printf("stream-> after computeStep \n"); 
#endif

  /*  
   * if HT flag is on, then 0 of compute rank 
   * writes results to csv file 
   * else if HT flag is off, then 0 of global rank 
   * writes results to csv file 
   */ 
  int computeRank;
  MPI_Comm_rank(computeComm, &computeRank); 

	// reduce results across computeComm and get max timers
	reduceResults(&streamParams, computeComm); 

	if(computeRank == 0)
	{
			resultsOutput(&streamParams, computeComm); // output avg timers to csv file 
			fullResultsOutput(&streamParams); // output all timers to csv files
	}   
	
#ifndef NDEBUG
  printf("stream-> after results output \n"); 
#endif

  MPI_Finalize(); 
#ifndef NDEBUG
  printf("MPI finalize\n"); 
#endif   

  return 0; 
} 

