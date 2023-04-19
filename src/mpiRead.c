#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/iocomp.h"

void mpiRead(struct iocomp_params *iocompParams, double* iodata_test)
{
	FILE *fp;
	size_t num; 
	fp = fopen(iocompParams->FILENAMES[iocompParams->ioLibNum],"r");
	int ioRank; 
	int ierr = MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
	mpi_error_check(ierr); 

	if (fp==NULL)
	{
		printf("Error: file %s not opening. Exiting \n", iocompParams->FILENAMES[iocompParams->ioLibNum]); 
		mpi_error_check(1); 
	}
	printf("file %s open \n", iocompParams->FILENAMES[iocompParams->ioLibNum]); 
	num = fread(iodata_test, sizeof(double), iocompParams->globalDataSize, fp);
	printf("fread \n"); 
	if(num!=iocompParams->globalDataSize)
	{
		printf("read elements %li not equal to global size of file %li \n", num, iocompParams->globalDataSize); 
		mpi_error_check(1); 
	}
	fclose(fp);
	printf("file closed \n"); 
#ifndef NDEBUG
		VERBOSE_1(ioRank,"file object closed\n"); 
#endif 
}
