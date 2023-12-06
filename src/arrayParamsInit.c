#include <stdio.h>
#include "../include/iocomp.h"

/*
 * Initialises the iocomp_params struct with array parameters; start point, end
 * point and global array.
 */
void arrayParamsInit(struct iocomp_params *iocompParams, MPI_Comm comm)
{

	if(iocompParams->NDIM == 2)
	{
		decomposition_2D(iocompParams, comm); 
	} 
	else if(iocompParams->NDIM == 1)
	{
		decomposition_1D(iocompParams, comm); 
	} 
	else
	{
		printf("NDIM not 1 or 2. Exiting \n");   
		exit(1); 
	} 
	
} 
