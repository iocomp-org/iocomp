#include "mpi.h"
#include "iocomp.h"
#define LOOPCOUNT 10 

struct test_variables{
	int LOOP_COUNT; 
	double startTime[LOOPCOUNT], endTime[LOOPCOUNT]; 
	size_t localDataSize; 
	size_t globalDataSize; 
	char* FILENAMES[5];  
	int computeRank; 
}; 
extern struct test_variables testParams; 
double* initialise(struct test_variables* testParams); 
void deleteFile(struct test_variables* testParams, int io); 
int rmdir(char *path); // delete the directory 
void rmfile(char* filename); // delete a file 
//void stats(struct test_variables* testParams); // stats function to output the write times, global size, local size, ranks 
bool checkData(int io, struct test_variables* testParams); // function to check values of files
bool mpiRead(struct test_variables* testParams); 
bool hdf5Read(struct test_variables* testParams); 
//int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf); 
