/** @file getFileName.c 
 *  @brief Gets the file name from the compute process.  
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */

#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>  
#include <string.h> 
#include <assert.h> 
#include <math.h>
#include "../include/iocomp.h"

/**
 * @file getFileName.c
 * @brief Gets the file name from the compute process using dynamic MPI message recieves.
 * Only used for splitting and shared route. 
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param i window number 
 * @return void
 */
void getFileName(struct iocomp_params *iocompParams, int i)
{
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "getFileName -> entered \n"); 
#endif 

	// only applicable for the splitting route 
	if(iocompParams->hyperthreadFlag == 1 || iocompParams->sharedFlag == 1)  
	{
		MPI_Status status;
		int size; 
		int ierr; 
		int source, tag; 
		MPI_Comm comm; 	
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "getFileName -> after if statement \n"); 
#endif 

		/*
		 * ht flag operations and shared flag operation have different sources and
		 * communicators.  
		 */ 
		if(iocompParams->hyperthreadFlag)
		{
			source = getPair(iocompParams); 
			int globalRank; 
			int globalSize; 
			MPI_Comm_rank(iocompParams->globalComm, &globalRank); 
			MPI_Comm_size(iocompParams->globalComm, &globalSize); 
			tag = globalSize; 
			comm = iocompParams->globalComm; 
		}
		else
		{
			source = 0; 
			tag = 0; 
			comm = iocompParams->newComm; 
		} 

		// Probe for an incoming message from compute process 
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "getFileName -> before MPI probe \n"); 
#endif 
		ierr = MPI_Probe(source, tag, comm, &status);
		mpi_error_check(ierr); 
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "getFileName -> MPI probe \n"); 
#endif 

		// When probe returns, get size of filename 
		ierr = MPI_Get_count(&status, MPI_CHAR, &size);
		mpi_error_check(ierr); 
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "getFileName -> MPI get count with size %i\n", size); 
#endif 
		
		// if HT flag activated then only 1 write file is needed which will be sent
		// to the IO libraries straight away 
		if(iocompParams->hyperthreadFlag)
		{
			i = 0; 
		}
		// Allocate a pointer to the double pointer plus space for extensions 
		iocompParams->writeFile[i] = (char*)malloc(sizeof(char) * (size));
		assert(iocompParams->writeFile[i]!=NULL); 
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "getFileName -> MPI write file malloced  \n");
#endif 

		// Receive the filename message with the allocated buffer
#ifdef VERBOSE 
		printf("getFileName->tag = %i, source = %i \n", tag, source); 
#endif 
		ierr = MPI_Recv(iocompParams->writeFile[i], size, MPI_CHAR, source, tag, comm, MPI_STATUS_IGNORE); 
		mpi_error_check(ierr); 
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "getFileName -> File name %s with size %i for window number %i \n", iocompParams->writeFile[i], size, i );
#endif 
	}
} 

