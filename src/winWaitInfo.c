#include "../include/iocomp.h"

void winWaitInfo(struct iocomp_params *iocompParams, double* array)
{	
	// get the window test flag for the array 
	for(int i = 0; i < iocompParams->numWin; i++)
	{
		if(iocompParams->array[i] == array)
		{
			iocompParams->wintestflags[i] = WIN_WAIT;  
		}
	} 
} 
