#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"
#define filename "write_time.csv"

void data_output(struct iocomp_params *iocompParams)
{
	int test; 
	FILE* out; 

#ifndef NDEBUG
	printf("remove filename \n");
#endif
	test = remove(filename);
	if (test != 0)
	{
#ifndef NDEBUG
		printf("Cant remove %s \n", filename);
#endif 
	}
	out = fopen(filename, "w+");
	if (out == NULL)
	{
		printf("Error: No output file\n");
		exit(1);
	}
	
	double writeRate = iocompParams->globalDataSize*sizeof(double)/( pow(10,9) * iocompParams->writeTime); 
	double localDataSize = iocompParams->localDataSize*sizeof(double)/pow(10,9); 
	double globalDataSize = iocompParams->globalDataSize*sizeof(double)/pow(10,9); 

	int ioSize, ioRank; 
	MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
	MPI_Comm_size(iocompParams->ioServerComm, &ioSize); 

	fprintf(out, "IOSize, WriteTime(s), LocalDataSize(GB), GlobalDataSize(GB), WriteRate(GB/s) \n"); //headers for output csv 
	fprintf(out, "%i, %lf,%lf,%lf,%lf \n",ioSize, iocompParams->writeTime, localDataSize, globalDataSize, writeRate); 
} 
