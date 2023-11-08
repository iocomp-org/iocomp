#include <string.h> 
#include "test.h"

void arguments(struct test_params* testParams, int argc, char** argv)
{
	
	// default arguments 
	testParams->nx = 10; 
	testParams->ny = 10; 
	testParams->io = 0; 
	testParams->verbose = 0; 
	
	// loop for command line arguments 
	for(int i = 0; i < argc; i++)
	{
		if(strcmp(argv[i], "--nx") == 0) 
		{
			testParams->nx = atoi(argv[i+1]);  
		}
		else if(strcmp(argv[i], "--ny") == 0) 
		{
			testParams->ny = atoi(argv[i+1]);  
		}
		else if(strcmp(argv[i], "--io") == 0) 
		{
			testParams->io = atoi(argv[i+1]);  
		}
		else if(strcmp(argv[i], "--v") == 0) 
		{
			testParams->verbose = 1; 
		}
	}
}
