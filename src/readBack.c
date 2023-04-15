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
	// print out data stream
	for(int i = 0; i < (int)iocompParams->globalDataSize; i++)
	{
		printf("%lf ", readData[i]); 
	}
	free(readData); 
	readData = NULL; 
} 
