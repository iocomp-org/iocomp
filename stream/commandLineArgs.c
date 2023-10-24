#include "iocomp.h"
#include "stream.h"
#include "string.h"
void commandLineArgs(struct stream_params* streamParams, int globalRank, int argc, char** argv)
{

	// default arguments 
	streamParams->HT_flag = 0; 
	streamParams->sharedFlag = 0;  
	streamParams->testFlag = 0;  
	streamParams->nx = 10; 
	streamParams->ny = 10; 
	streamParams->io = 0; 
	streamParams->verboseFlag = 0; 

	// loop for command line arguments 
	for(int i = 0; i < argc; i++)
	{
		if(strcmp(argv[i], "--HT") == 0) 
		{
			streamParams->HT_flag = true; 
		} 
		else if(strcmp(argv[i], "--nx") == 0) 
		{
			streamParams->nx = atoi(argv[i+1]);  
		}
		else if(strcmp(argv[i], "--ny") == 0) 
		{
			streamParams->ny = atoi(argv[i+1]);  
		}
		else if(strcmp(argv[i], "--io") == 0) 
		{
			streamParams->io = atoi(argv[i+1]);  
		}
		else if(strcmp(argv[i], "--shared") == 0) 
		{
			streamParams->sharedFlag = true;  
		}
		else if(strcmp(argv[i], "--verbose") == 0) 
		{
			streamParams->verboseFlag = atoi(argv[i+1]);  
		}
	}

	if(streamParams->verboseFlag == 0)
	{
		streamParams->debug = stdout; 
	} 
	else
	{
		snprintf(streamParams->debugFile, sizeof(streamParams->debugFile), "STREAM_RANK_%i.out", globalRank); 

		// remove existing file 
		remove(streamParams->debugFile);

		// create file in append mode 
		streamParams->debug = fopen(streamParams->debugFile,"a");
		if(streamParams->debug == NULL)
		{
			mpi_error_check(EXIT_FAILURE); 
		}
	} 
}
