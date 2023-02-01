#include "mpi.h"
#include "iocomp.h"

double* initialise(struct iocomp_params* iocompParams, MPI_Comm comm); 
void deleteFile(int ioname); 
