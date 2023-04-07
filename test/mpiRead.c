#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "test.h"

bool mpiRead(struct test_variables* testParams)
{
	printf("Testing file %s ...\n", testParams->FILENAMES[0]); 

	FILE *fp;
	double* iodata_test = NULL; 
	iodata_test = (double*)malloc(testParams->globalDataSize*sizeof(double));
	size_t num; 
	fp = fopen(testParams->FILENAMES[0], "r");
	printf("Testing for file presence ... \n"); 

	if (fp==NULL)
	{
		printf("Error: file %s not opening. Exiting \n", testParams->FILENAMES[0]); 
		exit(1); 
	}
	printf("Testing for file presence passed \n"); 

	num = fread(iodata_test, sizeof(double), testParams->globalDataSize, fp);

	arrayVerification(testParams, iodata_test); // element wise checking 

	fclose(fp);
#ifndef NDEBUG
		printf("file object closed\n"); 
#endif 
	free(iodata_test);
#ifndef NDEBUG
		printf("iodata freed\n"); 
#endif 
	iodata_test = NULL;
	printf("Testing from file %s complete\n", testParams->FILENAMES[0]); 
	return 0; 
}