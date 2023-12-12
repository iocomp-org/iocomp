#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>  
#include <assert.h> 
#include <math.h>
#include "iocomp.h"

int main(int argc, char** argv)
{
	int ierr;
	ierr = MPI_Init(&argc, &argv);  
	mpi_error_check(ierr); 

	/*	
	 * initialise stream param structs using command line parameters
	 */ 
	MPI_Comm comm = MPI_COMM_WORLD; 
	int rank; 
	int size; 
	MPI_Comm_rank(comm, &rank); 
	MPI_Comm_size(comm, &size); 
	// data parameters definitions 

	if(!rank){
		printf("arrayparamstesting \n"); 
	}
	struct iocomp_params test; 
	int NX=10;	
	int NY=10;	
	test.localDataSize = size*NX*NY; 
	test.NDIM = 2; 
	arrayParamsInit(&test, comm); 

	printf("Rank %i localDataSize = %li \n", rank, test.localDataSize);
	printf("arrayParamsInit-> globalArray:[%lu,%lu] \n", test.globalArray[0], test.globalArray[1] );  
	printf("arrayParamsInit-> localArray: [%lu,%lu] \n", test.localArray[0],  test.localArray[1] );  
	printf("arrayParamsInit-> arrayStart: [%lu,%lu] \n", test.arrayStart[0],  test.arrayStart[1] );

	MPI_Finalize(); 
	if(!rank){
		printf("Testing ends\n"); 
	} 
} 


