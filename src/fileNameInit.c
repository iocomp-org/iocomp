#include <stdio.h>
#include <stdlib.h>  
#include <string.h> 
#include "../include/iocomp.h"
// each window can write to its own file, initialise write file name for
// each window number 

void fileNameInit(struct iocomp_params *iocompParams, char filenames[NUM_WIN][100])
{

		char EXT[10]; 

		// Get correct extension based on I/O library used 
		switch(iocompParams->ioLibNum)
		{
			case 0:
				strcpy(EXT, ".dat"); 
				break; 
			case 1:
				strcpy(EXT, ".h5"); 
				break; 
			case 2:
				strcpy(EXT, ".h5"); 
				break; 
			case 3:
				strcpy(EXT, ".bp4"); 
				break; 
			case 4:
				strcpy(EXT, ".bp5"); 
				break; 
			default:
				printf("ioLibNum invalid, invalid extension applied \n"); 
				break; 
		} 
		
		// assign filename per window and per iteration in the format
		// windowname_iteration.ext
		for(int i = 0; i < NUM_WIN; i++)
		{
			for(int j = 0; j < AVGLOOPCOUNT; j++)
			{
				char iter[5]; 
				sprintf(iter,  "%d", j);
				strcpy(iocompParams->WRITEFILE[i][j], filenames[i]); 
				strcat(iocompParams->WRITEFILE[i][j], "_"); 
				strcat(iocompParams->WRITEFILE[i][j], iter); 
				strcat(iocompParams->WRITEFILE[i][j], EXT); 
#ifndef NDEBUG 
				fprintf(iocompParams->debug, "fileNameInit-> Window num %i, loop counter %i, filename %s \n",i, j, iocompParams->WRITEFILE[i][j]); 
#endif 
			} 
		}

} 
