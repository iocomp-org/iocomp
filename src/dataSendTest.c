/** @file dataSendTest.c
 *  @brief Implements data send testing for async I/O.
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "../include/iocomp.h"

/**
 * @file dataSendTest.c
 * @brief Implements data send testing for async I/O.
 * In case of HT flag, MPI Test is used. 
 * In case of shared flag, array sent in the function call are assigned WIN TEST constant. 
 * @param iocompParams pointer to struct containing all parameters for the library
 * @param request pointer to MPI_Request object
 * @param array pointer to array to wait for
 * @return void
 */
int dataSendTest(struct iocomp_params *iocompParams,MPI_Request *request, double* array)
{
	if(iocompParams->hyperthreadFlag) // only implement if HT flag switched on 
	{
		int mpiWaitFlag; 
		int ierr = MPI_Test(request,&mpiWaitFlag,MPI_STATUS_IGNORE); // MPI test 
		mpi_error_check(ierr); 
		return(mpiWaitFlag);
	}
	else if(iocompParams->sharedFlag)
	{
		for(int i = 0; i < iocompParams->numWin; i++)
		{
			if(iocompParams->array[i] == array)
			{
				iocompParams->wintestflags[i] = WIN_TEST;  
			}
		} 
		return -1; 
	} 
	else
	{
		return -1; 
	} 
} 

