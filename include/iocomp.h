#include "mpi.h"
#include <stdbool.h>
#define ioColour 0
#define compColour 1 
#define writeLoops 10 
#define ioLibCount 1 
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
// sizes and ranks
bool hyperthreadFlag;  
int colour; 
int ioServerRank; 
int compServerRank;
int ioServerSize; 
int compServerSize;
// for data_output function 
double writeTime[ioLibCount][writeLoops]; 
char ioLibs[ioLibCount][100]; 
// for io_libraries function 
int NDIM; 
int* localSize; 
int* arrayStart; 
int* globalSize; 
int globalDataSize; 
int localDataSize; 
MPI_Datatype dataType; 
}; 
extern struct iocomp_params iocompParams; 

void ioLibraries(double* iodata, struct iocomp_params *iocompParams); 
void timing_int(double time_diff, double global_data_size, int irep, int MAXLOOP_AVGIO, int local_n, char* filename, MPI_Comm comm);
void dataSend(double* data, struct iocomp_params *iocompParams); 
void ioServer(struct iocomp_params *iocompParams); 
int getPair(struct iocomp_params *iocompParams); 
//double* init_data(int N, int NDIM, MPI_Comm globalComm); 
//void comm_split(int color, MPI_Comm globalComm, MPI_Comm *splitComm);
void compute_comm_create(int color, MPI_Comm splitComm, MPI_Comm *computeComm);
void comm_split(struct iocomp_params *iocompParams, MPI_Comm comm); 
void data_output(struct iocomp_params *iocompParams);  
void arrayParamsInit(struct iocomp_params *iocompParams, MPI_Comm comm, int NDIM, int* localArraySize); 
void iocompInit(struct iocomp_params *iocompParams, MPI_Comm comm, int NDIM, int* localArraySize, bool flag); // initialises the library 
void testData(struct iocomp_params *iocompParams, int testFlag); // test data structures with flag to switch on/off  

// checks 
void mpi_error_check(int ierr);
void malloc_check(double* test); 
void free_check(double* test); 

/*
* IO libraries headers 
*/ 

void phdf5write(double* iodata, int*local_size, int* global_size, int* arraystart, int NDIM, MPI_Comm cartcomm, char* FILENAME);  
void mpiiowrite(double* iodata, int*local_size, int* global_size, int* arraystart, int NDIM, MPI_Comm cartcomm, char* FILENAME, MPI_Datatype dataType);  
//void adioswrite(double* iodata, int*arraysubsize, int* arraygsize, int* arraystart, int NDIM, MPI_Comm cartcomm, char *IOENGINE, char* FILENAME); 

