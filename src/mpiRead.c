#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/iocomp.h"

double* mpiRead(struct iocomp_params *iocompParams)
{
	FILE *fp;
	double* iodata_test = NULL; 
	size_t filesize; 
	filesize = iocompParams->globalArray[0] * iocompParams->globalArray[1]; 
	iodata_test = (double*)malloc(filesize*sizeof(double));
	size_t num; 
	fp = fopen(iocompParams->FILENAMES[iocompParams->ioLibNum],"r");

	if (fp==NULL)
	{
		printf("Error: file %s not opening. Exiting \n", iocompParams->FILENAMES[iocompParams->ioLibNum]); 
		exit(1); 
	}
	printf("file %s open \n", iocompParams->FILENAMES[iocompParams->ioLibNum]); 
	num = fread(iodata_test, sizeof(double), filesize, fp);
	printf("fread \n"); 
	if(num!=filesize)
	{
		printf("read elements %li not equal to global size of file %li \n", num, iocompParams->globalDataSize); 
		mpi_error_check(1); 
	}
	fclose(fp);
	printf("file closed \n"); 
#ifndef NDEBUG
		printf("file object closed\n"); 
#endif 
	return iodata_test; 
}
