#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include <string.h> 
#include "../include/iocomp.h"

#define OUTPUTFILE "iocomp_writetimes.csv"

void printWriteTimers(struct iocomp_params *iocompParams)
{
	double maxWriteTimes[100]; // as write times are defined till 100 numbers. 
	for(int i=0; i < 100; i++)
	{
		maxWriteTimes[i] = 0.0; 
	} 
	// Reduce write times across I/O server to get the max for all the iterations
	int ierr = MPI_Reduce(&iocompParams->writeTime, &maxWriteTimes, iocompParams->writeCount, MPI_DOUBLE, MPI_MAX, 0, iocompParams->ioServerComm); 
	mpi_error_check(ierr); 
	
	int ioRank; 
	MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 

	// Print reduced timers to file by IO rank 0 
	if(!ioRank)
	{
		// remove if already existing 
		remove(OUTPUTFILE);

		// create file in append mode 
		FILE* fptr; 
		fptr = fopen(OUTPUTFILE,"a");
		if(fptr == NULL)
		{
			mpi_error_check(EXIT_FAILURE); 
			exit(1); 
		}
		
		// print to file 
		fprintf(fptr, "%s \n", "Counter, writing time(s)"); 
		for(int counter = 0; counter < iocompParams->writeCount; counter++)
		{
			fprintf(fptr, "%i, %lf \n", counter, maxWriteTimes[counter]); 
		} 
		fclose(fptr); 
	} 
} 

	
