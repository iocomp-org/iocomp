#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"
#include "getopt.h"
#define NDIM 2

static int verbose_flag;
static int HT_flag; 

int main(int argc, char** argv)
{

  int ierr;
  ierr = MPI_Init(&argc, &argv);  
  mpi_error_check(ierr); 

  /*
   * Command line arguments 
   */ 
  /* Flag set by ‘--verbose’. */

  int c;

  while (1)
  {
    static struct option long_options[] =
    {
      /* These options set a flag. */
      {"verbose", no_argument,       &verbose_flag, 0},
      {"HT",   no_argument,       &HT_flag, 1},
      /* These options don’t set a flag.
         We distinguish them by their indices. */
      {0, 0, 0, 0}
    };
    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "abc:d:f:",						long_options, &option_index);

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
      puts ("HT flag is set to on");
    else 
      puts ("HT flag is switched off"); 
  } 

  struct iocomp_params iocompParams; 
  int ioLib = 0; 

	/*
	 * iocomp - iocompInit initialises the ioServer 
	 * and initialises the compute comm 
	 */ 
  MPI_Comm computeComm = iocompInit(&iocompParams,comm, HT_flag, ioLib); 
#ifndef NDEBUG
  printf("stream->After intercommInit\n"); 
#endif

	/*	
	 * initialise stream param struct  
	 */ 
	struct stream_params streamParams; 
	
	/*
	 * initialises the local array sizes 
	 * and data size 
	 */ 
  int localArraySize[NDIM] = {128,128}; 
	streamParams.localDataSize = 1;  
	for(int j = 0; j < NDIM; j++) 
	{
		streamParams.localDataSize *= localArraySize[j]; 
	}
#ifndef NDEBUG
  printf("stream-> localdatasize initialised with %li \n", streamParams.localDataSize); 
#endif

  computeStep(&iocompParams, &streamParams, comm); // do compute 
#ifndef NDEBUG
  printf("stream-> after computeStep \n"); 
#endif
	
	int computeRank; 
	MPI_Comm_rank(computeComm, &computeRank); 
	if(computeRank == 0)
	{
		resultsOutput(&streamParams); // write to csv file for compute write 
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

