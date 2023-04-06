#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "test.h"

void arrayVerification(struct test_variables* testParams, double* iodata)
{
	printf("Testing for array values ... \n"); 
	for ( int i = 0; i < testParams->globalDataSize; i++)
	{
		if(iodata[i]!=testParams->CONSTANT)
		{
			printf("Error: value at index %i not matching. Exiting \n", i); 
			exit(1); 
		}
	}
	printf("Testing for array values passed \n"); 
}
