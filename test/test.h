#include "mpi.h"
#include "iocomp.h"

double* initialise(struct iocomp_params* iocompParams, MPI_Comm comm); 
void deleteFile(int ioname); 
int rmdir(char *path); // delete the directory 
//int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf); 
