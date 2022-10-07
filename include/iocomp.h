#include "mpi.h"
#define ioColour 0
#define compColour 1 
/*
* structure passes around to most functions in program 
*/ 
struct iocomp_params 
{
// setting up of interComms 
MPI_Comm globalComm; 
MPI_Comm ioServerComm; 
MPI_Comm compServerComm; 
MPI_Comm interComm; 
int colour; 
int ioServerRank; 
int compServerRank;
int ioServerSize; 
int compServerSize;
// for data_output function 
double writeTime; 
// for io_libraries function 
int NDIM; 
int* localSize; 
int* arrayStart; 
int* globalSize; 
int globalDataSize; 
int localDataSize; 
}; 
extern struct iocomp_params iocompParams; 

//void intercommInit(struct iocomp_params *iocompParams, MPI_Comm comm);  
void intercommInit(struct iocomp_params *iocompParams, MPI_Comm comm, int NDIM, int* localArraySize);
void mpi_error_check(int ierr);
//void io_libraries(double* iodata, int NDIM, int* local_size, int* global_size,int* array_start,  MPI_Comm comm); 
void ioLibraries(double* iodata, struct iocomp_params *iocompParams); 
void timing_int(double time_diff, double global_data_size, int irep, int MAXLOOP_AVGIO, int local_n, char* filename, MPI_Comm comm);
void computeServer(double* data, struct iocomp_params *iocompParams); 
void ioServer(struct iocomp_params *iocompParams); 
double* initData(struct iocomp_params *iocompParams); 
void intercomm(struct iocomp_params *iocompParams); 
//double* init_data(int N, int NDIM, MPI_Comm globalComm); 
//void comm_split(int color, MPI_Comm globalComm, MPI_Comm *splitComm);
void compute_comm_create(int color, MPI_Comm splitComm, MPI_Comm *computeComm);
double* init_data(int N, int NDIM, MPI_Comm globalComm);
void intercomm_create(struct iocomp_params *iocompParams) ; 
void comm_split(struct iocomp_params *iocompParams);  
void data_output(struct iocomp_params *iocompParams);  

/*
* IO libraries headers 
*/ 

void phdf5write(double* iodata, int*local_size, int* global_size, int* arraystart, int NDIM, MPI_Comm cartcomm, char* FILENAME);  
void mpiiowrite(double* iodata, int*local_size, int* global_size, int* arraystart, int NDIM, MPI_Comm cartcomm, char* FILENAME);  
//void adioswrite(double* iodata, int*arraysubsize, int* arraygsize, int* arraystart, int NDIM, MPI_Comm cartcomm, char *IOENGINE, char* FILENAME); 

/*
* test 
*/ 
void computeStep(double* iodata, struct iocomp_params *iocompParams); 
