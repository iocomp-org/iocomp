#include "mpi.h"


void mpi_error_check(int ierr);
void bench_init(double* iodata, int NDIM, int* size, int* global_size,int* array_start,  MPI_Comm comm); 
void timing_int(double time_diff, double global_data_size, int irep, int MAXLOOP_AVGIO, int local_n, char* filename, MPI_Comm comm);
void computeServer(int NDIM, int* local_size, MPI_Comm computeComm, MPI_Comm interComm); 
void ioServer(int NDIM, int* local_size, MPI_Comm interComm, MPI_Comm ioServeComm, MPI_Comm globalComm);
void intercomm(MPI_Comm comm); 

/*
* individual files for linking without benchmark_c
*/ 

//void phdf5write(double* iodata, int*local_size, int* global_size, int* arraystart, int NDIM, MPI_Comm cartcomm, char* FILENAME);  
//void mpiiowrite(double* iodata, int*local_size, int* global_size, int* arraystart, int NDIM, MPI_Comm cartcomm, char* FILENAME);  
//void adioswrite(double* iodata, int*arraysubsize, int* arraygsize, int* arraystart, int NDIM, MPI_Comm cartcomm, char *IOENGINE, char* FILENAME); 
