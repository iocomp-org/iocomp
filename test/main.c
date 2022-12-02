#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "test.h"
#include "getopt.h"

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

  // check for HT flag 
  if (HT_flag)
    puts ("HT flag is set to on");
  else 
    puts ("HT flag is switched off"); 

  MPI_Comm comm; 
  MPI_Comm_dup(MPI_COMM_WORLD,&comm); 
  int rank; 
  MPI_Comm_rank(comm, &rank); 
  // data parameters definitions 

  int NDIM = 2; 
  int localArraySize[2] = {1000,1000}; 

  struct iocomp_params iocompParams; 

  iocompInit(&iocompParams, comm,  NDIM, localArraySize, HT_flag); 
#ifndef NDEBUG
  printf("After intercommInit\n"); 
#endif

  double* data = NULL; // initialise data pointer  
  data = (double*)malloc(iocompParams.localDataSize*sizeof(double)); // one rank only sends to one rank
  computeStep(data,&iocompParams); // do compute 
#ifndef NDEBUG
  printf("after computeStep \n"); 
#endif
  if(rank == 0)
  {
    int testFlag = 1; 
    //				testData(&iocompParams, testFlag); // test data 
  } 
#ifndef NDEBUG
  printf("after test\n"); 
#endif

  MPI_Finalize(); 
#ifndef NDEBUG
  printf("MPI finalize\n"); 
#endif   

  // remove file 
  if(rank == 0)
  {
    int ret; 
    ret = remove("mpiio.dat");

    if(ret != 0) {
      printf("Error: unable to delete the file");
    }
  } 
#ifndef NDEBUG
  printf("Deleted file\n"); 
#endif   
  free(data); 
  data = NULL; 

  return 0; 
} 

