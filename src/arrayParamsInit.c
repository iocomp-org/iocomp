/** @file arrayParamsInit.c
 *  @brief Initialises array variables based on local size by decomposing total size into NDIM dimensions.
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */

#include <stdio.h>
#include "../include/iocomp.h"

/**
 * @file arrayParamsInit.c
 * @brief Initialises array variables based on local size by decomposing total size into NDIM dimensions.
 * Initialises the iocomp_params struct with array parameters; start point, endpoint and global array.
 * @param iocompParams pointer to struct containing all parameters for the library
 * @return void
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
