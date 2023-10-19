#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>  
#include <string.h> 
#include <assert.h> 
#include <math.h>
#include "../include/iocomp.h"

void verify(struct iocomp_params *iocompParams, MPI_Comm comm)
{
	MPI_Barrier(comm); 
	printf("Verification testing started \n"); 

	// adios2 initialise object and i/o variable 
	if(iocompParams->ioLibNum >=2 && iocompParams->ioLibNum <= 4)
	{
#if ADIOS2_USE_MPI
		iocompParams->adios_read = adios2_init_config_mpi(CONFIG_FILE_ADIOS2, iocompParams->cartcomm);  
#else 
		iocompParams->adios_read = adios2_init();  
#endif 
		adios2_set_engine(iocompParams->io,ioParams->ADIOS2_IOENGINES[ioParams->ioLibNum-2]); 
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
	char windowName[] = {"B", "C", "A"}; 

	char readFile[3][10]; 


	for(int iter = 0; iter < AVGLOOPCOUNT; iter++)
	{
		// get stream expected output for that iteration
		b = SCALAR * c; 
		c = a + b; 
		a = b + (SCALAR*c); 
#ifndef NDEBUG
		fprintf(iocompParams->debug, "a[%i] = %lf, b[%i] = %lf, c[%i] = %lf \n", iter, a, iter, b, iter, c); 
#endif 

		// read data from the different windows; 
		// b, c, a is the order of windows 
		for(int windowNum = 0; windowNum < NUM_WIN; windowNum++)
		{

			// different IO libraries called depending on IO parameter 
			// file name extension changed depending on IO library 
			if(streamParams->io == 0)
			{
				snprintf(readFile[windowNum], sizeof(readFile[i]), "%s_%i.dat", windowName[windowNum], iter);
				mpiRead(readData, FILENAME, iocompParams); 
			} 
			else if(streamParams->io == 1)
			{
				snprintf(readFile[windowNum], sizeof(readFile[i]), "%s_%i.h5", windowName[windowNum], iter);
				phdf5Read(readData, FILENAME, iocompParams); 
			} 
			else if( (streamParams->io == 2) || (streamParams->io == 3) || (streamParams->io == 4))
			{
				if(streamParams->io == 2)
				{
					snprintf(readFile[windowNum], sizeof(readFile[i]), "%s_%i.h5", windowName[windowNum], iter);
				}
				else
				{
					snprintf(readFile[windowNum], sizeof(readFile[i]), "%s_%i", windowName[windowNum], iter);
				} 
				adios2Read(readData, FILENAME, iocompParams); 
			} 

			// verify data by checking value by value with STREAM code   
			int test = valueCheck(iocompParams, readData, val, readFile[windowNum]); 
			int test_reduced;  
#ifndef NDEBUG   
			fprintf(iocompParams->debug,"Filename %s Data read: \n", ioParams->WRITEFILE[windowNum][iter]); 
			for(int i = 0; i < iocompParams->localDataSize; i++)
			{
				fprintf(iocompParams->debug,"%lf, ", readData[i]); 
			}
			fprintf(iocompParams->debug,"\n"); 
#endif       

			// sync all values of test, if multiplication comes back as 0 it means
			// verification failed by a particular rank 
			MPI_Reduce(&test, &test_reduced, 1, MPI_INT, MPI_PROD, 0, comm); 
			if(!myRank)
			{
				if(test_reduced == 0)
				{
					printf("Verification failed \n"); 
				} 
				else
				{
					printf("Verification passed for filename %s \n", FILENAME); 
				}
			}
		} 
	} 

	free(readData); 
	readData = NULL; 
#ifndef NDEBUG   
	fprintf(iocompParams->debug,"iodata test freed\n"); 
#endif       
	MPI_Barrier(iocompParams->ioComm); 

} 

