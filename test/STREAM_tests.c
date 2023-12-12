#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>  
#include <string.h> 
#include <assert.h> 
#include <math.h>
#include "test.h"

void STREAM_tests(struct test_params *testParams, struct iocomp_params *iocompParams,  MPI_Comm comm)
{ 
	int myRank; 
	MPI_Comm_rank(comm, &myRank); 
	double a , b, c, val; 

	int passed, failed; 
	passed = 0; 
	failed = 0; 

	// initialise data buffer to store data read locally 
	double* read_ptr = NULL; 

	// Initialise 
	a = 1.0;
	b = 2.0; 
	c = 0.0; 

	// filenames for the different windows 
	char readFile[3][100]; 

	char ext[5][10] = {".dat", ".h5", ".h5", "", ""}; 

	for(int iter = 0; iter < AVGLOOPCOUNT; iter++)
	{
		// get stream expected output for that iteration
		b = CONSTANT * c; 
		c = a + b; 
		a = b + (CONSTANT*c); 

		// read data from the different windows; 
		// b, c, a is the order of windows 
		// obtain value from the window numbers
		for(int windowNum = 0; windowNum < NUMWIN; windowNum++)
		{
			if(windowNum == 0)
			{
				val = b; 
				snprintf(readFile[windowNum], sizeof(readFile[windowNum]), "B_%i%s",iter, ext[testParams->io]);
			} 
			else if(windowNum == 1)
			{
				val = c; 
				snprintf(readFile[windowNum], sizeof(readFile[windowNum]), "C_%i%s",iter, ext[testParams->io]);
			} 
			else
			{
				val = a; 
				snprintf(readFile[windowNum], sizeof(readFile[windowNum]), "A_%i%s",iter, ext[testParams->io]);
			} 

			if( (myRank==0) && (testParams->verbose==true) ){
				printf("Verification tests starting for filename %s \n", readFile[windowNum]); 
			} 
			// read data from readFile  
			read_ptr = readFiles(testParams, iocompParams, readFile[windowNum]); 
			if(read_ptr == NULL)
			{
				if( (myRank==0) && (testParams->verbose==true) ){
					printf("Filename %s does not exist. \n", readFile[windowNum]); 
				} 
				failed++; 
				break; 
			} 

			// verify data by checking value by value with STREAM code simulator,
			// value returned tells if the tests passed 
			int test = valueCheck(iocompParams, read_ptr, val, readFile[windowNum]); 

			// sync all values of test, if multiplication comes back as 0 it means
			// verification failed by a particular rank 
			int test_reduced = 0; 
			MPI_Reduce(&test, &test_reduced, 1, MPI_INT, MPI_PROD, 0, comm); 

			// if all processes return true then verification passes. 
			if(test_reduced == 0)
			{
				if( (myRank==0) && (testParams->verbose==true) )
				{	
					printf("Verification failed for filename %s \n", readFile[windowNum]); 
				} 
				failed++; 
			} 
			else
			{
				if( (myRank==0) && (testParams->verbose==true) )
				{	
					printf("Verification passed for filename %s \n", readFile[windowNum]); 
				} 
				passed++; 
			}
#ifndef NODELETE
			if(myRank==0) 
      {
        if(testParams->verbose==true)
        {	
          printf("Deleting file %s \n", readFile[windowNum]); 
        } 
				deleteFilesTest(readFile[windowNum]); 
			} 
			MPI_Barrier(comm); 
#endif 
      free(read_ptr); 
      read_ptr = NULL; 
		} 
	} 

	if(!myRank) {
		printf("Verification tests finished. %i tests passed and %i tests failed. \n", passed, failed); 
	} 
} 
