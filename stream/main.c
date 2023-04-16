#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"
#include "getopt.h"

static int verbose_flag;
static int HT_flag; 
static int nx; 
static int ny; 
static int size1; 
static int size2; 
static int io; 

int main(int argc, char** argv)
{

  int ierr;
  ierr = MPI_Init(&argc, &argv);  
  mpi_error_check(ierr); 

  /*
   * Command line arguments using getoptlong 
	 * HT option switches on comm splitting 
	 * size option can set size of array during runtime 
   */ 

  int c;
	size1 = 10; 
	size2 = 10; 
	io = 0; 
  while (1)
  {
    static struct option long_options[] =
    {
      /* These options set a flag. */
      {"verbose", no_argument,       &verbose_flag, 0},
      {"HT",   no_argument,       &HT_flag, 1},
      /* These options don’t set a flag.
         We distinguish them by their indices. */
			{"size1",  required_argument, 0, 'd'}, 
			{"size2",  required_argument, 0, 'e'}, 
			{"io",  required_argument, 0, 'f'}, 
      {0, 0, 0, 0}
    };
    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "d:e:f:",						long_options, &option_index);

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
				size1 = atoi(optarg); 

			case 'e':
				size2 = atoi(optarg); 

			case 'f':
				io = atoi(optarg); 

      case '?':
        /* getopt_long already printed an error message. */
        break;

      default:
        abort ();
    }
  }

  MPI_Comm comm; 
  MPI_Comm_dup(MPI_COMM_WORLD,&comm); 
  int rank; 
  MPI_Comm_rank(comm, &rank); 
  // data parameters definitions 
		  
  if(!rank){
    // check for HT flag 
    if (HT_flag)
		{
      puts ("HT flag is set to on");
			printf("size of array %i x %i IO num %i \n", size1, size2, io); 
		} 
    else 
		{
      puts ("HT flag is switched off"); 
			printf("size of array %i x %i IO num %i \n", size1,size2, io); 
		} 
  } 

  struct iocomp_params iocompParams; 

	/*
	 * iocomp - iocompInit initialises the ioServer 
	 * and initialises the compute comm 
	 */ 
  MPI_Comm computeComm = iocompInit(&iocompParams,comm, HT_flag, io, NODESIZE); 
#ifndef NDEBUG
  printf("stream->After intercommInit\n"); 
#endif

	/*	
	 * initialise stream param struct  
	 */ 
	struct stream_params streamParams; 
	
	/*
	 * initialises the local array sizes 
	 * multiplying by size1 x size2
	 */ 
	streamParams.localDataSize = size1*size2; 
#ifndef NDEBUG
  printf("stream-> localdatasize initialised with %li \n", streamParams.localDataSize); 
#endif

  computeStep(&iocompParams, &streamParams, computeComm); // do compute 
#ifndef NDEBUG
  printf("stream-> after computeStep \n"); 
#endif

  /*  
   * if HT flag is on, then 0 of compute rank 
   * writes results to csv file 
   * else if HT flag is off, then 0 of global rank 
   * writes results to csv file 
   */ 
  int computeRank;
  MPI_Comm_rank(computeComm, &computeRank); 

	// reduce results across computeComm and get max timers
	reduceResults(&streamParams, computeComm); 

	if(computeRank == 0)
	{
			resultsOutput(&streamParams, computeComm); // output avg timers to csv file 
			fullResultsOutput(&streamParams); // output all timers to csv files
	}   
	
#ifndef NDEBUG
  printf("stream-> after results output \n"); 
#endif

  MPI_Finalize(); 
#ifndef NDEBUG
  printf("MPI finalize\n"); 
#endif   

  return 0; 
} 

