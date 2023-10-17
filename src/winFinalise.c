#include "../include/iocomp.h"
// if shared MPI used free window frees the arrays. Not the case with the HT
// flag switched on  
void winFinalise(struct iocomp_params *iocompParams)
{
	if(iocompParams->hyperthreadFlag == true)
	{
		for(int i = 0; i < NUM_WIN; i++)
		{
			free(iocompParams->array[i]); 
			iocompParams->array[i] = NULL; 
		} 
	} 
} 
