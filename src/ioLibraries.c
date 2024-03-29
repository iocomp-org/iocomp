/**
 * @file ioLibraries.c
 * @brief Passes data to I/O libraries and backends
 * 
 * @author Shrey Bhardwaj (sb1595)
 * @bug No known bugs.
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <sys/time.h>
#include <string.h>
#include "../include/iocomp.h"

/**
 * @brief Passes data to I/O libraries and backends
 * 
 * Passes data to I/O libraries and backends. 
 * 
 * @param iodata pointer to data to be written
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param windowNum window number
 * @return void
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
#ifndef NOTIMERS
			iocompParams->timerStart[iocompParams->writeCount] = MPI_Wtime(); 
#endif 
			mpiiowrite(iodata, iocompParams, windowNum);
#ifndef NOTIMERS
			iocompParams->writeTime[iocompParams->writeCount] = MPI_Wtime() - iocompParams->timerStart[iocompParams->writeCount];
#endif 
			break; 
#ifndef NOHDF5
		case 1: 
#ifndef NOTIMERS
			iocompParams->timerStart[iocompParams->writeCount] = MPI_Wtime();
#endif 
			phdf5write(iodata, iocompParams, windowNum);
#ifndef NOTIMERS
			iocompParams->writeTime[iocompParams->writeCount] = MPI_Wtime() - iocompParams->timerStart[iocompParams->writeCount]; 
#endif 
			break; 
#endif 
			/*
			 * in case of adios2, initialisations of different engines obtained using config file
			 * which takes place in ioServerInitialise
			 */ 
#ifndef NOADIOS2
		case 2: case 3: case 4: 
#ifndef NOTIMERS
			iocompParams->timerStart[iocompParams->writeCount] = MPI_Wtime();
#endif 
			adioswrite(iodata, iocompParams, windowNum);
#ifndef NOTIMERS
			iocompParams->writeTime[iocompParams->writeCount] = MPI_Wtime() - iocompParams->timerStart[iocompParams->writeCount];
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

	iocompParams->writeCount++; // increment write counter at end of loop 
}

