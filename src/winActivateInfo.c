/** @file winActivateInfo.c
 *  @brief Assigns WIN_ACTIVATE flag to the window corresponding to the array
 * 	so that I/O server knows to activate the window. 
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */
#include "../include/iocomp.h"

/**
 * @file winActivateInfo.c
 * @brief Assigns WIN_ACTIVATE flag to the window corresponding to the array. 
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param array pointer to array to be activated
 * @return void
 */
void winActivateInfo(struct iocomp_params *iocompParams, double* array)
{
	for(int i = 0; i < iocompParams->numWin; i++)
	{
		if(iocompParams->array[i] == array)
		{
			iocompParams->wintestflags[i] = WIN_ACTIVATE;  
		}
	}
} 
