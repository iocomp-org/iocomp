#include "../include/iocomp.h"
#include <stdlib.h>
#include "stdio.h"

void malloc_check(double* test)
{
	if(test == NULL)
	{	
		printf("Malloc fails, exiting. \n"); 
		exit(1); 
	}

}

void free_check(double* test)
{
	if(test != NULL)
	{
		printf("free fails, exiting \n"); 
		exit(1); 
	}

}

/*
 * Given an error code, line number and file name, check the error code.
 * If error code is non-zero print an error message and exit with that value.
 */
void error_report_fn(int ierr, int line_no, char *file_name)
{
	if (ierr != 0)
	{
		printf("ERROR: received error code %i at line %i of %s\n", ierr, line_no, file_name);
		exit(ierr);
	}
}
