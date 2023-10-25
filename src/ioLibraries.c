#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <sys/time.h>
#include <string.h>
#include "../include/iocomp.h"

/*
 * Purpose: define local, global, offsets for data for particular rank 
 * Then send this to the different I/O libraries. 
 */ 
void ioLibraries(double* iodata, struct iocomp_params *iocompParams, int windowNum)
{
	int ioSize, ioRank;

	MPI_Comm_size(iocompParams->ioServerComm, &ioSize);
	MPI_Comm_rank(iocompParams->ioServerComm, &ioRank);
	
#ifdef VERBOSE
	fprintf(iocompParams->debug,"ioLibraries -> Pass off to I/O libraries with ioLibNum %i  \n", iocompParams->ioLibNum);
#endif

	switch(iocompParams->ioLibNum){

		case 0:
#ifdef IOCOMP_TIMERS
			double timerStart; 
			if (!ioRank) {timerStart = MPI_Wtime();} 
#endif 
			strcat(iocompParams->writeFile[windowNum], ".dat"); 
			mpiiowrite(iodata, iocompParams, windowNum);
#ifdef IOCOMP_TIMERS
			MPI_Barrier(iocompParams->ioServerComm);
			if (!ioRank) {writeTime = MPI_Wtime() - timerStart;} 
#endif 
			break; 
#ifndef NOHDF5
		case 1: 
#ifdef IOCOMP_TIMERS
			if (!ioRank) {timerStart = MPI_Wtime();} 
#endif 
			strcat(iocompParams->writeFile[windowNum], ".h5"); 
			phdf5write(iodata, iocompParams, windowNum);
#ifdef IOCOMP_TIMERS
			MPI_Barrier(iocompParams->ioServerComm);
			if (!ioRank) {writeTime = MPI_Wtime() - timerStart;} 
#endif 
			break; 
#endif 
			/*
			 * in case of adios2, initialisations of different engines obtained using config file
			 * which takes place in ioServerInitialise
			 */ 
#ifndef NOADIOS2
		case 2: case 3: case 4: 
#ifdef IOCOMP_TIMERS
			if (!ioRank) {timerStart = MPI_Wtime();} 
#endif 
			// adios2 calling hdf5 writes to a file with h5 extension 
			if(iocompParams->ioLibNum==2)
			{
				strcat(iocompParams->writeFile[windowNum], ".h5"); 
			}
			adioswrite(iodata, iocompParams, windowNum);
#ifdef IOCOMP_TIMERS
			MPI_Barrier(iocompParams->ioServerComm);
			if (!ioRank) {writeTime = MPI_Wtime() - timerStart;} 
#endif 
			break; 
#endif 

		default:
			printf("Invalid I/O library number \n"); 
			break; 
	} 
#ifdef VERBOSE
	fprintf(iocompParams->debug,"ioLibraries -> end of switch for IO libraries\n");
#endif
#ifdef READBACK
	MPI_Barrier(iocompParams->ioServerComm); // wait for all processes to finish writing data 
	if(!ioRank)
	{
		readBack(iocompParams); // read files and print them out 
	} 
#endif

#ifdef IOCOMP_TIMERS
	if (!ioRank) 
	{
		double fileSize = iocompParams->globalDataSize*sizeof(double)/(pow(10,9)); 
		printf("** I/O write time=%lf filesize(GB)=%lf\n", writeTime,fileSize) ; 
	} 
#endif
}

