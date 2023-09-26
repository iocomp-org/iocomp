#include "../include/iocomp.h"
void winFreeInfo(struct iocomp_params *iocompParams, double* array)
{
	for(int i = 0; i < NUM_WIN; i++)
	{
		if(iocompParams->array[i] == array)
		{
			iocompParams->wintestflags[i] = WIN_FREE;  
		}
	}
} 
