#include <stdbool.h>
#include <stdlib.h>
#include "stdio.h"
#include "test.h"

void deleteFile(int ioname)
{
	// remove file 
	char* filenames[5] = {
		"mpiio.dat",
		"hdf5.h5",
		"output.h5",
		"output.bp4",
		"output.bp5"
	}; 
	
	if(ioname<4) // if not adios2, then its a file. 
	{
		int ret; 
		ret = remove(filenames[ioname-1]);
		if(ret != 0) {
			printf("Error: unable to delete the file");
		}
	} 
	else // if its adios2, then its a directory
	{
		int test; 
		test = rmdir(filenames[ioname-1]); 
	}

#ifndef NDEBUG
	printf("Deleted file\n"); 
#endif   
} 
