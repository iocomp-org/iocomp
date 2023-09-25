#include "../include/iocomp.h"
#include "assert.h" 

void dataSendInfo(struct iocomp_params *iocompParams)
{	
	// check if test flags have a value between -1 and 4 before broadcasting 
	for(int i = 0; i < NUM_WIN; i++)
	{
		assert(iocompParams->wintestflags[i] > -2); 
		assert(iocompParams->wintestflags[i] < 5); 
	}

#ifndef NDEBUG 
	fprintf(iocompParams->debug, "dataSend -> Before MPI bcast, iocompParams->wintestflags "); 
	for(int i = 0; i < NUM_WIN; i++)
	{
		fprintf(iocompParams->debug, "[%i]", 
			iocompParams->wintestflags[i]); 
	} 
	fprintf(iocompParams->debug, "\n"); 
#endif 
	// broadcast them to ioserver 
	MPI_Bcast( iocompParams->wintestflags, NUM_WIN, MPI_INT, 0, iocompParams->newComm); 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "dataSend -> after MPI bcast, iocompParams->wintestflags "); 
	for(int i = 0; i < NUM_WIN; i++)
	{
		fprintf(iocompParams->debug, "[%i]", 
			iocompParams->wintestflags[i]); 
	} 
	fprintf(iocompParams->debug, "\n"); 
#endif 
} 

void dataSendStart(struct iocomp_params *iocompParams, double* array)
{
	// get the window test flag for the array 
	for(int i = 0; i < NUM_WIN; i++)
	{
		if(iocompParams->array[i] == array)
		{
			// match array with windows and issue win complete 
			MPI_Win_start(iocompParams->group, 0, iocompParams->winMap[i]); 
		}
	}
} 

void dataSendEnd(struct iocomp_params *iocompParams, double* array)
{
	// get the window test flag for the array 
	for(int i = 0; i < NUM_WIN; i++)
	{
		if(iocompParams->array[i] == array)
		{
			// match array with windows and issue win complete 
			MPI_Win_complete(iocompParams->winMap[i]); 
		}
	}
} 
