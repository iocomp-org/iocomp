#include "mpi.h"
#include "iocomp.h"
#define LOOPCOUNT 10 

struct test_variables{
	int LOOP_COUNT; 
	double startTime[LOOPCOUNT], endTime[LOOPCOUNT]; 
	size_t localDataSize; 
	char* FILENAMES[5];  
}; 
extern struct test_variables testParams; 
double* initialise(struct test_variables* testParams); 
void deleteFile(struct test_variables* testParams, int io); 
int rmdir(char *path); // delete the directory 
void rmfile(char* filename); // delete a file 
void stats(struct test_variables* testParams); // stats function to output the write times, global size, local size, ranks 

//int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf); 
