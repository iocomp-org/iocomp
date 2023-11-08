/** @file checks.c	
 *  @brief Contains functions for checking malloc and free, reporting error line no. 
 *
 *  @author Shrey Bhardwaj (sb15895)
 *  @bug No know bugs.
 */
#include "../include/iocomp.h"
#include <stdlib.h>
#include "stdio.h"

/**
 * @file checks.c
 * @brief Contains functions for checking malloc.
 * @param test pointer to array to check
 * @return void
 */
void malloc_check(double* test)
{
	if(test == NULL)
	{	
		printf("Malloc fails, exiting. \n"); 
		exit(1); 
	}

}

/**
 * @file checks.c
 * @brief Contains functions for checking free.
 * @param test pointer to array to check
 * @return void
 */
void free_check(double* test)
{
	if(test != NULL)
	{
		printf("free fails, exiting \n"); 
		exit(1); 
	}

}

/**
 * @file error_check.c
 * @brief Checks error code and exits if non-zero.
 * @param ierr error code
 * @param line_no line number of error
 * @param file_name name of file where error occurred
 * @return void
 */
void error_report_fn(int ierr, int line_no, char *file_name)
{
	if (ierr != 0)
	{
		printf("ERROR: received error code %i at line %i of %s\n", ierr, line_no, file_name);
		exit(ierr);
	}
}
