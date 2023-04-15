#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "../include/iocomp.h"

void readBack(struct iocomp_params* iocompParams) 
{
	double* readData = NULL; 
	switch(iocompParams->ioLibNum)
	{
		case 0: 
			readData = mpiRead(iocompParams); 
			break; 

		case 1: 
			readData = hdf5Read(iocompParams); 
			break; 

		case 2: case 3: case 4: 
			readData = adios2Read(iocompParams); 
			break; 

		 default:
				break; 
	}
	// print out data stream into 2D
	for(int i = 0; i < (int)iocompParams->globalArray[0]; i++)
	{
		for(int j = 0; j < (int)iocompParams->globalArray[1]; j++)
		{
			printf("%lf,", readData[i*(int)iocompParams->globalArray[1] + j]); 
		} 
		printf("\n");  
	}
	free(readData); 
	readData = NULL; 
} 
