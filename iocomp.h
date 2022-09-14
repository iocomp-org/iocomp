#include "mpi.h"


void mpi_error_check(int ierr);
void bench_init(double* iodata, int NDIM, int* local_size, int* global_size,int* array_start,  MPI_Comm comm); 
void timing_int(double time_diff, double global_data_size, int irep, int MAXLOOP_AVGIO, int local_n, char* filename, MPI_Comm comm);
void computeServer(int NDIM, double* data, int* local_size, MPI_Comm computeComm, MPI_Comm interComm); 
void ioServer(int NDIM, int* local_size, MPI_Comm interComm, MPI_Comm ioServeComm, MPI_Comm globalComm);
void intercomm(MPI_Comm comm, double* data, int serverSize); 
double* init_data(int N, int NDIM, MPI_Comm globalComm); 
//void comm_split(int color, MPI_Comm globalComm, MPI_Comm *splitComm);
void compute_comm_create(int color, MPI_Comm splitComm, MPI_Comm *computeComm);
double* init_data(int N, int NDIM, MPI_Comm globalComm);
void intercomm_create(int color, MPI_Comm globalcomm, MPI_Comm computecomm, MPI_Comm ioservecomm, MPI_Comm *intercomm); 
void comm_split(int color, MPI_Comm globalComm, MPI_Comm *computeComm, MPI_Comm *ioServerComm); 
void computeSendData(double* send_ar, int localDataSize, int dest, MPI_Comm interComm, MPI_Comm computeComm);

/*
* individual files for linking without benchmark_c
*/ 

//void phdf5write(double* iodata, int*local_size, int* global_size, int* arraystart, int NDIM, MPI_Comm cartcomm, char* FILENAME);  
void mpiiowrite(double* iodata, int*local_size, int* global_size, int* arraystart, int NDIM, MPI_Comm cartcomm, char* FILENAME);  
//void adioswrite(double* iodata, int*arraysubsize, int* arraygsize, int* arraystart, int NDIM, MPI_Comm cartcomm, char *IOENGINE, char* FILENAME); 
