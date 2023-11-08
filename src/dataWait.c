/** @file dataWait.c
 *  @brief Implements waiting for data to be sent from client before modification of array.
 * 	Also sends file name to I/O process.
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */

#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

/**
 * @file dataWait.c
 * @brief Implements waiting for data to be sent from client before modification of array.
 * In case of HT flag, MPI Wait is used and file name is sent to I/O process. 
 * In case of shared flag, array sent in the function call are assigned WIN WAIT constant. 
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param request pointer to MPI_Request object
 * @param array pointer to array to wait for
 * @param fileName name of file to wait for
 * @return void
 */
void dataWait(struct iocomp_params *iocompParams,MPI_Request *request, double* array, char* fileName)
{

	if(iocompParams->hyperthreadFlag) // only implement if HT flag switched on 
	{
		int compRank; 
		MPI_Comm_rank(iocompParams->compServerComm, &compRank); 
#ifdef VERBOSE
		fprintf(iocompParams->debug,"dataWait -> MPI wait activated\n"); 
#endif
		// wait for the data to be sent from previous dataSend
		int ierr = MPI_Wait(request, MPI_STATUS_IGNORE);  
		mpi_error_check(ierr); 
		
		// send file name for the data after array has been sent 
		sendFileName(iocompParams, fileName); 
	}
	else if(iocompParams->sharedFlag)
	{
		// assign WIN WAIT to array window 
		for(int i = 0; i < iocompParams->numWin; i++)
		{
			if(iocompParams->array[i] == array)
			{
				iocompParams->wintestflags[i] = WIN_WAIT;  
			}
		} 
	}

} 

