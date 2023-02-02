#include "mpi.h"
#include <stdbool.h>
#include <stddef.h>

#define ioColour 0
#define compColour 1 
#define ioLibCount 4
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
double writeTime; 
char ioLibs[ioLibCount][100]; 
int ioLibNum; // select ioLib 
// for io_libraries function 
int NDIM; 
int* localSize; 
int* arrayStart; 
int* globalSize; 
size_t globalDataSize; 
size_t localDataSize; 
MPI_Datatype dataType; 
// filenames 
char *FILENAMES[5]; 
}; 
extern struct iocomp_params iocompParams; 

void ioLibraries(double* iodata, struct iocomp_params *iocompParams); 
void timing_int(double time_diff, double global_data_size, int irep, int MAXLOOP_AVGIO, int local_n, char* filename, MPI_Comm comm);
void dataSend(double* data, struct iocomp_params *iocompParams, MPI_Request *request); // function to send data using MPI_Isend 
void ioServer(struct iocomp_params *iocompParams); 
int getPair(struct iocomp_params *iocompParams); 
void compute_comm_create(int color, MPI_Comm splitComm, MPI_Comm *computeComm);
void comm_split(struct iocomp_params *iocompParams, MPI_Comm comm); 
void arrayParamsInit(struct iocomp_params *iocompParams, MPI_Comm comm, int NDIM, int* localArraySize); 
void iocompInit(struct iocomp_params *iocompParams, MPI_Comm comm, int NDIM, int* localArraySize, bool flag, int ioLibNum); // initialises the library 
void ioServerInitialise(struct iocomp_params *iocompParams, int ioLibNum); // initialise ioServer if ioProcessor 
void testData(struct iocomp_params *iocompParams, int testFlag); // test data structures with flag to switch on/off  
void stopSend(struct iocomp_params *iocompParams); // ghost send function that signals MPI_Sends are stopping
void dataWait(struct iocomp_params *iocompParams, MPI_Request *request); // wrapper function to implement waiting of data IF HT flag is activated 
int dataSendTest(struct iocomp_params *iocompParams,MPI_Request *request); // wrapper function to implement testing of send data IF HT flag is on 

// checks 
void mpi_error_check(int ierr);
void malloc_check(double* test); 
void free_check(double* test); 

/*
* IO libraries headers 
*/ 

void phdf5write(double* iodata, int*local_size, int* global_size, int* arraystart, int NDIM, MPI_Comm cartcomm, char* FILENAME);  
void mpiiowrite(double* iodata, int*local_size, int* global_size, int* arraystart, int NDIM, MPI_Comm cartcomm, char* FILENAME, MPI_Datatype dataType);  
void adioswrite(double* iodata, int*arraysubsize, int* arraygsize, int* arraystart, int NDIM, MPI_Comm cartcomm, char *IOENGINE, char* FILENAME); 

