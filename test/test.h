#include <mpi.h>
#include <adios2_c.h>

// #include "../include/iocomp.h"
#include "../stream/stream.h"

// test parameters structure 
struct test_params{
	int LOOP_COUNT; 
	// sizes
	size_t localDataSize; 
	size_t globalDataSize; 
	char* fullResults_filename[KERNELS]; 
	int writeFreq; // compute steps per writing
	int numWrites; // max number of writes  

	// command line args 
	bool HT_flag, sharedFlag, testFlag; 
	int nx, ny, io; 

	// file object for debug 
	char debugFile[100]; 
	FILE* debug; 
	int verboseFlag; 
}; 
extern struct test_params testParams; 

void arguments(struct test_params* testParams, int argc, char** argv); 
int valueCheck(struct iocomp_params *iocompParams, double* iodata_test, double val, char* filename); 
void STREAM_tests(struct test_params *testParams, struct iocomp_params *iocompParams,  MPI_Comm comm); 
double* readTest(struct test_params *testParams, struct iocomp_params *iocompParams, char* fileName,  MPI_Comm comm); 
bool file_exists (char *filename); 
void deleteFilesTest(char* fileName); 

