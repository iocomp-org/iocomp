#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>  
#include <string.h> 
#include <assert.h> 
#include <math.h>
#include <adios2_c.h>
//#include "../stream/stream.h"
//#include "../include/iocomp.h"
#include "test.h"
#define CONFIG_FILE_ADIOS2 "config.xml"

// small fn to check if file exists
#include <sys/stat.h>   // stat
#include <stdbool.h>    // bool type
bool file_exists (char *filename) {
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}


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
	 * initialise stream param structs using command line parameters
	 */ 
	commandLineArgs(&streamParams, argc, argv); 

	MPI_Comm comm = MPI_COMM_WORLD; 
	int rank; 
	int size; 
	MPI_Comm_rank(comm, &rank); 
	MPI_Comm_size(comm, &size); 
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
	// MPI_Comm computeComm = iocompInit(&iocompParams,comm, streamParams.HT_flag, streamParams.io, NODESIZE, streamParams.sharedFlag, NUMWIN); 
	printf("after iocompInit \n"); 
	
	// first set of tests: file is read, values are checked. 
	readBackTests( &streamParams, MPI_COMM_WORLD); 
	printf("after readbacktests \n"); 

	MPI_Finalize(); 
} 

