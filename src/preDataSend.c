#include "../include/iocomp.h"
#include "assert.h" 
#include <string.h>


void preDataSend(struct iocomp_params *iocompParams, double* array, char* fileName)
{
	if(iocompParams->sharedFlag)
	{
#ifndef NDEBUG 
		fprintf(iocompParams->debug, "preDataSend -> Before MPI bcast, iocompParams->wintestflags"); 
		for(int i = 0; i < iocompParams->numWin; i++)
		{
			fprintf(iocompParams->debug, "[%i]", 
					iocompParams->wintestflags[i]); 
		} 
		fprintf(iocompParams->debug, "\n"); 
#endif 

		// check if test flags have a value between -1 and 4 before broadcasting 
		for(int i = 0; i < iocompParams->numWin; i++)
		{
			assert(iocompParams->wintestflags[i] > -2); 
			assert(iocompParams->wintestflags[i] < 5); 
		}

		// broadcast them to ioserver 
		MPI_Bcast( iocompParams->wintestflags, iocompParams->numWin, MPI_INT, 0, iocompParams->newComm); 

#ifndef NDEBUG 
		fprintf(iocompParams->debug, "preDataSend -> After MPI broadcast  \n"); 
#endif 

		// get the window test flag for the array 
		for(int i = 0; i < iocompParams->numWin; i++)
		{
			if(iocompParams->array[i] == array)
			{
				/*
				 * Send file name from Comp server to I/O server before win start, send
				 * size + 1 for end character 
				 */ 
				int ierr = MPI_Send(fileName, ((int)strlen(fileName)+1), MPI_CHAR, 1, 0, iocompParams->newComm);  
				mpi_error_check(ierr); 
#ifndef NDEBUG 
				fprintf(iocompParams->debug, "file name sent %s with length %i\n", fileName, ((int)strlen(fileName)+1)) ; 
#endif 

				// match array with windows and issue win complete 
#ifndef NDEBUG 
				fprintf(iocompParams->debug, "preDataSend-> Before mpi win start for window %i \n", i); 
#endif 
				ierr = MPI_Win_start(iocompParams->group, 0, iocompParams->winMap[i]); 
				mpi_error_check(ierr); 
#ifndef NDEBUG 
				fprintf(iocompParams->debug, "preDataSend-> After mpi win start for window %i \n", i); 
#endif 
			}
		}
	}
	else 
	{
		//		/*
		//		 * First get the number of the array and then send the array 
		//		 */ 
		//		for(int i = 0; i < iocompParams->numWin; i++)
		//		{
		//			if(iocompParams->array[i] == array)
		//			{
		//				if(iocompParams->hyperthreadFlag)
		//				{
		//					/*
		//					 * For HT version, with splitting, Send file name from Comp server 
		//					 * to I/O server before win start, send size + 1 for end character. 
		//					 */ 
		//					int dest, tag; 
		//					dest = getPair(iocompParams); // destination for sending data. 
		//					int globalRank; 
		//					ierr = MPI_Comm_rank(iocompParams->globalComm, &globalRank);
		//					mpi_error_check(ierr); 
		//					tag = globalRank ;
		//#ifndef NDEBUG 
		//					fprintf(iocompParams->debug, "preDataSend->Going to send filename to dest %i with tag %i\n", dest, tag) ; 
		//#endif 
		//					int ierr = MPI_Send(fileName, ((int)strlen(fileName)+1), MPI_CHAR, dest, tag, iocompParams->globalComm);  
		//					mpi_error_check(ierr); 
		//#ifndef NDEBUG 
		//					fprintf(iocompParams->debug, "preDataSend->file name sent %s with length %i\n", fileName, ((int)strlen(fileName)+1)) ; 
		//#endif 
		//				} 
		/*
		 * For non splitting version, the library simply assigns the value to the
		 * structure. 
		 */ 
		iocompParams->writeFile[0] = fileName; 
	} 
} 

