#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "test.h"
#include "getopt.h"
#define LOOP_COUNT 1
#define MAXIO 2

static int verbose_flag;
static int HT_flag; 
static int size; 
static int io; 

int main(int argc, char** argv)
{
	struct iocomp_params iocompParams; // struct for iocomp library 
	struct test_variables testParams; // struct for test program
	double* data; 
	int globalRank, computeRank, computeSize; 

	int ierr;
	ierr = MPI_Init(&argc, &argv);  
	mpi_error_check(ierr); 

	MPI_Comm comm; 
	MPI_Comm_dup(MPI_COMM_WORLD,&comm); 
	MPI_Comm_rank(comm, &globalRank); 
	MPI_Request request; 
	int c;
	// get size, HT flag and I/O lib num from command line  
	while (1)
	{
		static struct option long_options[] =
		{
			/* These options set a flag. */
			{"verbose", no_argument,       &verbose_flag, 0},
			{"HT",   no_argument,       &HT_flag, 1},
			/* These options don’t set a flag.
				 We distinguish them by their indices. */
			{"size",  required_argument, 0, 'd'}, 
			{"io",  required_argument, 0, 'e'}, 
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "d:e:",						long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c)
		{
			case 0:
				/* If this option set a flag, do nothing else now. */
				if (long_options[option_index].flag != 0)
					break;
				printf ("option %s", long_options[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
				printf ("\n");
				break;

			case 'd':
				size = atoi(optarg); 

			case 'e':
				io = atoi(optarg); 

			case '?':
				/* getopt_long already printed an error message. */
				break;

			default:
				abort ();
		}
	}

	// 0 for non splitting and 1 for splitting of comms 
#ifndef NDEBUG
	printf("HT flag %i and io library %i \n", HT_flag, io); 
#endif 

	// initialise array sizes
	testParams.localDataSize = pow(size,2); 
	testParams.CONSTANT = 3; 
	MPI_Comm computeComm = iocompInit(&iocompParams, comm,  HT_flag, io); 
#ifndef NDEBUG
	printf("iocomp initialised \n"); 
#endif 
	MPI_Comm_rank(computeComm, &computeRank); 
	testParams.computeRank = computeRank; 
	MPI_Comm_size(computeComm, &computeSize); 
	testParams.globalDataSize = testParams.localDataSize * computeSize; 

	data = initialise(&testParams); // initialise array 
	for(int i = 0; i < LOOP_COUNT; i++) // send data, wait for data, check for data and loop
	{
		testParams.startTime[i] = MPI_Wtime();  
		dataSend(data,&iocompParams,&request,testParams.localDataSize); // send data off using dataSend
#ifndef NDEBUG
		printf("data send from rank %i\n", computeRank); 
		for(int j = 0; j<testParams.localDataSize; j++)
		{
			printf("%lf, ", data[j]); 
		}
		printf("\n"); 
		printf("data send \n"); 
#endif 
		dataWait(&iocompParams,&request);
		testParams.endTime[i] = MPI_Wtime(); 
#ifdef CHECK_DATA
		if(!computeRank)
		{
			ierr = checkData(io, &testParams);
			mpi_error_check(ierr); 
		} 
#endif
	} 
	stopSend(&iocompParams); // stop iocomp
#ifdef DEL_FILE
	if(!globalRank){
		deleteFile(&testParams, io); // delete file after I/O ops 
	} 
#endif 

	free(data); 
	data = NULL; 
	MPI_Finalize(); 
	return 0; 
} 


