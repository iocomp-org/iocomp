#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>  
#include <string.h> 
#include <assert.h> 
#include <math.h>
#include <adios2_c.h>
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
	struct test_params testParams; 

	/*	
	 * initialise stream param structs using command line parameters
	 */ 
	arguments(&testParams, argc, argv); 
	testParams.localDataSize = testParams.nx * testParams.ny;

	MPI_Comm comm = MPI_COMM_WORLD; 
	int rank; 
	int size; 
	MPI_Comm_rank(comm, &rank); 
	MPI_Comm_size(comm, &size); 
	// data parameters definitions 

	if(!rank){
		printf("size of array %i x %i IO num %i \n", testParams.nx, testParams.ny, testParams.io); 
	} 

	/*
	 * iocomp - iocompInit initialises the ioServer 
	 * and initialises the compute comm 
	 * HT flag and shared flag have been turned off. 
	 * Compute Comm is the same as WORLD COMM  
	 */ 
	MPI_Comm computeComm = iocompInit(&iocompParams,comm, 0, testParams.io, NODESIZE, 0, NUMWIN); 
	ioServerInitialise(&iocompParams);
	arrayParamsInit(&iocompParams, comm); 
	
	// first set of tests: file is read, values are checked. 
	STREAM_tests(&testParams, &iocompParams,  MPI_COMM_WORLD); 
	
	MPI_Finalize(); 
} 

