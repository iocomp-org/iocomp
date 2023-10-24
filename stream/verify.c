#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>  
#include <string.h> 
#include <assert.h> 
#include <math.h>
#include "stream.h"
#include "../include/iocomp.h"
#define CONFIG_FILE_ADIOS2 "config.xml"

// small fn to check if file exists
#include <sys/stat.h>   // stat
#include <stdbool.h>    // bool type
bool file_exists (char *filename) {
	struct stat   buffer;   
	return (stat (filename, &buffer) == 0);
}

void verify(struct iocomp_params *iocompParams, struct stream_params* streamParams, MPI_Comm comm)
{
	printf("Verification testing started \n"); 

	int myRank; 
	MPI_Comm_rank(comm, &myRank); 

	/* 
	 * for simplicity initialise IO communicator with the compute communicator 
	 * Initialise cartesian comm
	 * Initiliase array offset and dimensions 
	 */ 
	iocompParams->ioServerComm = comm;
	ioServerInitialise(iocompParams);
	arrayParamsInit(iocompParams, comm); 

	//  adios2 initialise object and i/o variable 
	if(iocompParams->ioLibNum >=2 && iocompParams->ioLibNum <= 4)
	{
#if ADIOS2_USE_MPI
		iocompParams->adios = adios2_init_config_mpi(CONFIG_FILE_ADIOS2, iocompParams->cartcomm);  
#else 
		iocompParams->adios = adios2_init();  
#endif 

		adios2_set_engine(iocompParams->io,iocompParams->ADIOS2_IOENGINES[iocompParams->ioLibNum-2]); 
	} 

	double a , b, c, val; 

	// initialise data buffer to store data read locally 
	double* readData; 
	readData = (double *) malloc(sizeof(double)*iocompParams->localDataSize); 

	// Initialise 
	a = 1.0;
	b = 2.0; 
	c = 0.0; 

	// filenames for the different windows 
	char windowName[3][2] = {"B", "C", "A"}; 
	// char readFile[3][10] = {"B", "C", "A"}; 

	char readFile[3][100]; 


	for(int iter = 0; iter < AVGLOOPCOUNT; iter++)
	{
		// get stream expected output for that iteration
		b = CONSTANT * c; 
		c = a + b; 
		a = b + (CONSTANT*c); 
		if(streamParams->verboseFlag){
			fprintf(streamParams->debug, "a[%i] = %lf, b[%i] = %lf, c[%i] = %lf \n", iter, a, iter, b, iter, c); 
		} 

		// read data from the different windows; 
		// b, c, a is the order of windows 
		for(int windowNum = 0; windowNum < NUMWIN; windowNum++)
		{

			// different IO libraries called depending on IO parameter 
			// file name extension changed depending on IO library 
			if(streamParams->io == 0)
			{
				snprintf(readFile[windowNum], sizeof(readFile[windowNum]), "%s_%i.dat", windowName[windowNum], iter);
				printf("filename %s \n", readFile[windowNum]); 
				if(file_exists(readFile[windowNum])) 
				{
					mpiRead(readData, readFile[windowNum], iocompParams); 
				} 
				else
				{
					printf("filename %s does not exist.\n", readFile[windowNum]); 
				}
			} 
			else if(streamParams->io == 1)
			{
				snprintf(readFile[windowNum], sizeof(readFile[windowNum]), "%s_%i.h5", windowName[windowNum], iter);
				printf("filename %s \n", readFile[windowNum]); 
				if(file_exists(readFile[windowNum])) 
				{
					phdf5Read(readData, readFile[windowNum], iocompParams); 
				} 
				else
				{
					printf("filename %s does not exist.\n", readFile[windowNum]); 
					exit(0); 
				}
			} 
			else if( (streamParams->io == 2) || (streamParams->io == 3) || (streamParams->io == 4))
			{
				if(streamParams->io == 2)
				{
					snprintf(readFile[windowNum], sizeof(readFile[windowNum]), "%s_%i.h5", windowName[windowNum], iter);
				}
				else
				{
					snprintf(readFile[windowNum], sizeof(readFile[windowNum]), "%s_%i", windowName[windowNum], iter);
				} 
				if(file_exists(readFile[windowNum])) 
				{
					adios2Read(readData, readFile[windowNum], iocompParams); 
				} 
				else
				{
					printf("filename %s does not exist.\n", readFile[windowNum]); 
					exit(0); 
				}
			} 

			// obtain value from the window numbers
			if(windowNum == 0)
			{
				val = b; 
			} 
			else if(windowNum == 1)
			{
				val = c; 
			} 
			else
			{
				val = a; 
			} 
			// verify data by checking value by value with STREAM code   
			int test = valueCheck(iocompParams, readData, val, readFile[windowNum]); 
			int test_reduced;  
			if(streamParams->verboseFlag){   
				fprintf(streamParams->debug,"Filename %s Data read: \n", readFile[windowNum]); 
				for(int i = 0; i < iocompParams->localDataSize; i++)
				{
					fprintf(streamParams->debug,"%lf, ", readData[i]); 
				}
				fprintf(streamParams->debug,"\n"); 
			}       

			// sync all values of test, if multiplication comes back as 0 it means
			// verification failed by a particular rank 
			MPI_Reduce(&test, &test_reduced, 1, MPI_INT, MPI_PROD, 0, comm); 
			if(!myRank)
			{
				if(test_reduced == 0)
				{
					printf("Verification failed \n"); 
					break; 
				} 
				else
				{
					printf("Verification passed for filename %s \n", readFile[windowNum]); 
				}
			}
		}
	} 


	free(readData); 
	readData = NULL; 
	if(streamParams->verboseFlag){   
		fprintf(streamParams->debug,"iodata test freed\n"); 
	}       
	printf("Verification tests finished \n"); 
	MPI_Barrier(comm); 
} 

