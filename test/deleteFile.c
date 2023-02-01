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

	int ret; 
	ret = remove(filenames[ioname]);
	if(ret != 0) {
		printf("Error: unable to delete the file");
	}

#ifndef NDEBUG
	printf("Deleted file\n"); 
#endif   
} 
