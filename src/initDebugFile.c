#include <stdio.h>
#include <stdlib.h>  
#include <string.h> 
#include <errno.h>
#include "../include/iocomp.h"

#ifndef NDEBUG
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
