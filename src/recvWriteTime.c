#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

void recvWriteTime(struct iocomp_params *iocompParams)
{
	double sum, avgWriteTime; 

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
	
	// MPI recv data from ioLibraries  
	double writeTime; 
	int size, source, tag;
	source  = 
	
	ierr = MPI_Recv(writeTime, size, MPI_DOUBLE, source, tag,
			iocompParams->globalComm,&status); 
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
