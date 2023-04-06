#include <stdbool.h>
#include <stdlib.h>
#include "stdio.h"
#include "test.h"

void rmfile(char* filename)
{
	int ret; 
	ret = remove(filename);
	if(ret != 0) {
		printf("Error: unable to delete the file");
	}
}
void deleteFile(struct test_variables* testParams, int ioLibNum)
{
	// remove file 
	if(ioLibNum<3) // if not adios2, then its a file. 
	{
		rmfile(testParams->FILENAMES[ioLibNum]); 
	} 
	else // if its adios2, then its a directory
	{
		int test; 
		test = rmdir(testParams->FILENAMES[ioLibNum]); 
	}

#ifndef NDEBUG
	printf("Deleted file\n"); 
#endif   
} 
