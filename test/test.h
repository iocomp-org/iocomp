#include <mpi.h>
#include <adios2_c.h>

// #include "../include/iocomp.h"
#include "../stream/stream.h"

int valueCheck(struct iocomp_params *iocompParams, double* iodata_test, double val, char* filename); 
void readBackTests(struct iocomp_params *iocompParams, struct stream_params *streamParams, MPI_Comm comm); 
bool file_exists (char *filename); 

