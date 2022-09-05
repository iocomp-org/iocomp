#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

int main(int argc, char** argv)
{

#ifndef NDEBUG
	printf("before mpi init main program \n"); 
#endif
	int ierr;
	ierr = MPI_Init(&argc, &argv);  
	mpi_error_check(ierr); 

	intercomm(MPI_COMM_WORLD); 
	
#ifndef NDEBUG
	printf("After intercomm program \n"); 
#endif

	MPI_Finalize(); 

#ifndef NDEBUG
	printf("MPI finalize\n"); 
#endif

	return 0; 
} 

