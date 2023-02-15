#include "mpi.h"
#include "iocomp.h"
#define iter 5
#define KERNELS 4
#define LOOPCOUNT 10

// number of iterations for averaging the stream tests 
//
struct stream_params{
	int LOOP_COUNT; 
	double timer_start; 
	double compTimer[KERNELS][LOOPCOUNT]; 
	double waitTimer[KERNELS][LOOPCOUNT]; 
	double sendTimer[KERNELS][LOOPCOUNT]; 
	double startTime[LOOPCOUNT], endTime[LOOPCOUNT]; 
	size_t localDataSize; 
	size_t globalDataSize; 
	int myrank; 
	int mysize;
	int ioLibNum; 
	char* FILENAMES[5];  
}; 
extern struct stream_params streamParams; 

void computeStep(struct iocomp_params *iocompParams, struct stream_params* streamParams);
void stream(double* iodata, struct iocomp_params *iocompParams, struct stream_params* streamParams); 
void initData(double* iodata, struct iocomp_params *iocompParams);
void resultsOutput(double timer[4][iter], double totalTimer[4][iter],double waitTimer[4][iter], double wallTimer); 
void avg(double sum[KERNELS], double data[KERNELS][iter]); 
double timer_start(int computeRank); 
double timer_end(double timerStart, int computeRank ); 
