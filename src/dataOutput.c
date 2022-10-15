#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"
#define filename "write_time.csv"

void data_output(struct iocomp_params *iocompParams)
{
	int test; 
	FILE* out; 
	double sum, avgWriteTime; 
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

	int globalSize, globalRank; 
	int ioSize, ioRank; 
	MPI_Comm_rank(iocompParams->globalComm, &globalRank); 
	MPI_Comm_size(iocompParams->globalComm, &globalSize); 

	if(iocompParams->hyperthreadFlag) // check if flag is true? 
	{
		MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
		MPI_Comm_size(iocompParams->ioServerComm, &ioSize); 
	}
	else
	{
		ioRank = globalRank; 
		ioSize = globalSize; 
	}
	
	// init the ioLib
	strcpy(iocompParams->ioLibs[0],"MPI"); 
	
	double writeRate, localDataSize, globalDataSize; 	
	fprintf(out, "IO library, Global Processors, IO Processors, LocalDataSize(GB), GlobalDataSize(GB),  WriteTime(s), WriteRate(GB/s) \n"); //headers for output csv 

	/*
	* avg the writetime 
	*/ 
	for (int k = 0; k < ioLibCount; k ++) 
	{
		for(int i = 0; i < writeLoops; i++)
		{
			sum = 0.0;
			sum += iocompParams->writeTime[k][i]; 
		}
		avgWriteTime = sum/writeLoops; 
		writeRate = iocompParams->globalDataSize*sizeof(double)/( pow(10,9) * avgWriteTime); 
		localDataSize = iocompParams->localDataSize*sizeof(double)/pow(10,9); 
		globalDataSize = iocompParams->globalDataSize*sizeof(double)/pow(10,9); 
		fprintf(out, "%s, %i,%i,%lf,%lf,%lf,%lf \n",iocompParams->ioLibs[k], globalSize, ioSize, localDataSize, globalDataSize, avgWriteTime, writeRate); 
	} 
} 
