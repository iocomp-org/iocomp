#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>
#include <math.h>
#include <sys/time.h>
#include "../include/iocomp.h"

/*
 * Purpose: define local, global, offsets for data for particular rank 
 * Then send this to the different I/O libraries. 
 */ 
void ioLibraries(double* iodata, struct iocomp_params *iocompParams)
{
#ifndef NDEBUG
	printf("ioLibraries -> start\n");
#endif
	int reorder = 0; 
	int ioSize, ioRank;
	int coords[iocompParams->NDIM], periods[iocompParams->NDIM],dims_mpi[iocompParams->NDIM];
	
	MPI_Comm_size(iocompParams->ioServerComm, &ioSize);
	MPI_Comm_rank(iocompParams->ioServerComm, &ioRank);

#ifndef NDEBUG
	printf("ioLibraries -> initialise filenames \n");
#endif

	double timerStart = 0.0; 
	
#ifndef NDEBUG
	printf("ioLibraries -> Pass off to I/O libraries with ioLibNum %i  \n", iocompParams->ioLibNum);
#endif

	switch(iocompParams->ioLibNum){

		case 0:
#ifdef IOCOMP_TIMERS
			if (!ioRank) {timerStart = MPI_Wtime();} 
#endif 
			mpiiowrite(iodata, iocompParams);
#ifdef IOCOMP_TIMERS
			MPI_Barrier(comm);
			if (!ioRank) {writeTime = MPI_Wtime() - timerStart;} 
#endif 
		break; 

		case 1: 
#ifdef IOCOMP_TIMERS
			if (!ioRank) {timerStart = MPI_Wtime();} 
#endif 
			phdf5write(iodata, iocompParams);
#ifdef IOCOMP_TIMERS
			MPI_Barrier(comm);
			if (!ioRank) {writeTime = MPI_Wtime() - timerStart;} 
#endif 
		break; 
	
		case 2: 
#ifdef IOCOMP_TIMERS
			if (!ioRank) {timerStart = MPI_Wtime();} 
#endif 
			adioswrite(iodata, iocompParams);
#ifdef IOCOMP_TIMERS
			MPI_Barrier(comm);
			if (!ioRank) {writeTime = MPI_Wtime() - timerStart;} 
#endif 
		break; 
	
		case 3: 
#ifdef IOCOMP_TIMERS
			if (!ioRank) {timerStart = MPI_Wtime();} 
#endif 
			adioswrite(iodata, iocompParams);
#ifdef IOCOMP_TIMERS
			MPI_Barrier(comm);
			if (!ioRank) {writeTime = MPI_Wtime() - timerStart;} 
#endif 
		break;

		case 4: 
#ifdef IOCOMP_TIMERS
			if (!ioRank) {timerStart = MPI_Wtime();} 
#endif 
			adioswrite(iodata, iocompParams);
#ifdef IOCOMP_TIMERS
			MPI_Barrier(comm);
			if (!ioRank) {writeTime = MPI_Wtime() - timerStart;} 
#endif 
		break; 

		default:
			printf("Invalid I/O library number \n"); 
			break; 
	} 
#ifndef NDEBUG
	printf("ioLibraries -> end of switch for IO libraries\n");
#endif

#ifdef IOCOMP_TIMERS
	if (!ioRank) 
	{
		double fileSize = iocompParams->globalDataSize*sizeof(double)/(pow(10,9)); 
		printf("** I/O write time=%lf filesize(GB)=%lf\n", writeTime,fileSize) ; 
	} 
#endif
}

