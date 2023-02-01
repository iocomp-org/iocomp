#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "test.h"

int main(int argc, char** argv)
{

  int ierr;
  ierr = MPI_Init(&argc, &argv);  
  mpi_error_check(ierr); 

  MPI_Comm comm; 
  MPI_Comm_dup(MPI_COMM_WORLD,&comm); 
  int rank; 
  MPI_Comm_rank(comm, &rank); 
  // data parameters definitions 

  struct iocomp_params iocompParams;
	double* data;
	// initialise function and populate iocompParams, returning data array to write. 
	data = initialise(&iocompParams, comm); 

	MPI_Request request; 
	dataSend(data,&iocompParams, &request); // send data off using dataSend

	int ioLibNum = 3; 
	ioServerInitialise(&iocompParams, ioLibNum); 
	dataWait(&iocompParams,&request);
  stopSend(&iocompParams); 

  MPI_Finalize(); 
#ifndef NDEBUG
  printf("MPI finalize\n"); 
#endif   
	
	if(rank == 0)	
	{
		deleteFile(ioLibNum); 
	} 
  free(data); 
  data = NULL; 
  return 0; 
} 

