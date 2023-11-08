/** @file initDebugFile.c	
 *  @brief Initialises the debug file.
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */

#include <stdio.h>
#include <stdlib.h>  
#include <string.h> 
#include <errno.h>
#include "../include/iocomp.h"

#ifdef VERBOSE
/**
 * @file initDebugFile.c
 * @brief Initialises the debug file.
 * Initialises the debug file if the VERBOSE flag is set. 
 * If the debug variable is not set to NULL, then it prints out debug statements to stdout.
 * Else, it will print all the debug statements to a file named like DEBUG_RANK_<GLOBAL RANK>.out 
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param globalRank rank of process in global communicator
 * @return void
 */
void initDebugFile(struct iocomp_params *iocompParams, int globalRank)
{
	iocompParams->debug = NULL;
	if(!iocompParams->debug)
	{
		// print out debug statements  
		iocompParams->debug = stdout;  
	} 
	else
	{
		strcpy(iocompParams->debugFile,"DEBUG_Rank_"); 
		
		// attach rank
		char stringRank[5]; 
		sprintf(stringRank,  "%d", globalRank); 
		strcat(iocompParams->debugFile, stringRank); 
		
		// attach extension 
		strcat(iocompParams->debugFile, ".out"); 

		remove(iocompParams->debugFile);
		
		// create file in append mode 
		iocompParams->debug = fopen(iocompParams->debugFile,"a");
		if(iocompParams->debug == NULL)
		{
			mpi_error_check(EXIT_FAILURE); 
			printf("Value of errno: %d\n", errno);
		}
	} 
} 
#endif 
