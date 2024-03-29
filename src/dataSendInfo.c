/** @file dataSendInfo.c	
 *  @brief Implements control mechanisms for the client process for shared method.
 * 	Win start, win complete and broadcasting to I/O server are implemented here.
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */

#include "../include/iocomp.h"
#include "assert.h" 

/**
 * @file dataSendInfo.c
 * @brief Sends wintestflags to I/O server for shared method.
 * @param iocompParams pointer to struct containing all parameters for the library
 * @return void
 */
void dataSendInfo(struct iocomp_params *iocompParams)
{	
	if(iocompParams->sharedFlag)
	{
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "dataSendInfo -> Before MPI bcast, iocompParams->wintestflags :"); 
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
#ifdef VERBOSE 
		fprintf(iocompParams->debug, "dataSendInfo -> After MPI bcast \n"); 
#endif 
	} 
} 

/**
 * @file dataSendInfo.c
 * @brief Calls MPI_Win_start for the corresponding array window.
 * @param iocompParams pointer to struct containing all parameters for the library
 * @return void
 */
void dataSendStart(struct iocomp_params *iocompParams, double* array)
{
	if(iocompParams->sharedFlag)
	{
		// get the window test flag for the array 
		for(int i = 0; i < iocompParams->numWin; i++)
		{
			if(iocompParams->array[i] == array)
			{
				// match array with windows and issue win complete 
#ifdef VERBOSE 
				fprintf(iocompParams->debug, "dataSendStart -> Before mpi win start for window %i \n ", i); 
#endif 
				int ierr = MPI_Win_start(iocompParams->group, 0, iocompParams->winMap[i]); 
				mpi_error_check(ierr); 
#ifdef VERBOSE 
				fprintf(iocompParams->debug, "dataSendStart -> After mpi win start for window %i \n ", i); 
#endif 
			}
		}
	} 
} 

/**
 * @file dataSendInfo.c
 * @brief Calls MPI_Win_complete for the corresponding array window.
 * @param iocompParams pointer to struct containing all parameters for the library
 * @return void
 */
void dataSendEnd(struct iocomp_params *iocompParams, double* array)
{
	if(iocompParams->sharedFlag)
	{
		// get the window test flag for the array 
		for(int i = 0; i < iocompParams->numWin; i++)
		{
			if(iocompParams->array[i] == array)
			{
				// match array with windows and issue win complete 
				MPI_Win_complete(iocompParams->winMap[i]); 
			}
		}
	} 
} 
