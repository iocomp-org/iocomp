#include "../include/iocomp.h" 
#include <errno.h>

void fileWrite(struct iocomp_params *iocompParams, double* iodata, int* loopCounter, int windowNum)
{	
#ifdef IOBW
	iocompParams->writeTime_start = MPI_Wtime(); 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "fileWrite->Before writing window:%i loopCounter %i, writeTime %lf, winTime %lf \n",windowNum, 
			loopCounter[windowNum], iocompParams->writeTime[windowNum][loopCounter[windowNum]], iocompParams->winTime[windowNum][loopCounter[windowNum]]); 
#endif 
#endif

#ifndef NDEBUG
	int ioRank; 
	MPI_Comm_rank(iocompParams->ioComm, &ioRank); 
	fprintf(iocompParams->debug, "fileWrite-> IO rank %i Writing to filename = %s loopCounter %i WindowNum %i\n", ioRank, iocompParams->WRITEFILE[windowNum][loopCounter[windowNum]], loopCounter[windowNum], windowNum);
#endif 

	// call io libraries 
	switch(iocompParams->ioLibNum)
	{
		case(0): 
			mpiiowrite(iodata,iocompParams->WRITEFILE[windowNum][loopCounter[windowNum]], iocompParams); 
			break; 
		case(1): 
			phdf5write(iodata,iocompParams->WRITEFILE[windowNum][loopCounter[windowNum]], iocompParams); 
			break; 
		case(2): case(3): case(4): 
			adioswrite(iodata, iocompParams->WRITEFILE[windowNum][loopCounter[windowNum]], iocompParams); 
			break; 
		default:
			printf("Invalid io number"); 
			break; 
	} 

#ifdef IOBW
	// finish write time 
	iocompParams->writeTime_end = MPI_Wtime(); 
	iocompParams->writeTime[windowNum][loopCounter[windowNum]] = iocompParams->writeTime_end - iocompParams->writeTime_start; 
	// finish window, and end win timer 
	iocompParams->winTime_end[windowNum] = MPI_Wtime(); 
	iocompParams->winTime[windowNum][loopCounter[windowNum]] = iocompParams->winTime_end[windowNum] - iocompParams->winTime_start[windowNum]; 
#ifndef NDEBUG 
	fprintf(iocompParams->debug, "fileWrite->After writing window:%i loopCounter %i, writeTime %lf, winTime %lf \n", windowNum, 
			loopCounter[windowNum], iocompParams->writeTime[windowNum][loopCounter[windowNum]], iocompParams->winTime[windowNum][loopCounter[windowNum]]); 
#endif 
#endif 
	// increment loopCounter after filewrite complete 
	loopCounter[windowNum]++; 
} 
