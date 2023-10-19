#include "iocomp.h"
#include "stream.h"
#include "string.h"


void commandLineArgs(struct stream_params* streamParams, int argc, char** argv)
{
	
	// default arguments 
	streamParams->HT_flag = 0; 
	streamParams->sharedFlag = 0;  
	streamParams->testFlag = 0;  
	streamParams->nx = 10; 
	streamParams->ny = 10; 
	streamParams->io = 0; 
	
	// loop for command line arguments 
	for(int i = 0; i < argc; i++)
	{
		if(strcmp(argv[i], "--HT") == 0) 
		{
			streamParams->HT_flag = 1; 
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
			streamParams->sharedFlag = 1;  
		}
		else if(strcmp(argv[i], "--test") == 0) 
		{
			streamParams->testFlag = 1;  
		}
	}
}
