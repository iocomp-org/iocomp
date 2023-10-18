#include "../include/iocomp.h"
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
