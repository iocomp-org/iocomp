#include "mpi.h"
#include "iocomp.h"

#define iter 5
#define KERNELS 4
#define LOOPCOUNT 10
#define COPY		0
#define SCALE		1
#define ADD			2
#define TRIAD		3

// number of iterations for averaging the stream tests 

struct stream_params{
	int LOOP_COUNT; 
	double timer_start; 
	double compTimer[KERNELS][LOOPCOUNT]; 
	double waitTimer[KERNELS][LOOPCOUNT]; 
	double sendTimer[KERNELS][LOOPCOUNT]; 
	size_t localDataSize; 
	size_t globalDataSize; 
	MPI_Request requestArray[KERNELS]; 
	int mpiWaitFlag[KERNELS]; 
}; 
extern struct stream_params streamParams; 
// stream functions 
void copy(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* a); 
void copy_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k); 
void add(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* a, double* b); 
void add_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k); 

void computeStep(struct iocomp_params *iocompParams, struct stream_params* streamParams, MPI_Comm comm);
void stream(double* iodata, struct iocomp_params *iocompParams, struct stream_params* streamParams, MPI_Comm comm); 
void initData(double* iodata, struct iocomp_params *iocompParams);
void resultsOutput(double timer[4][iter], double totalTimer[4][iter],double waitTimer[4][iter], double wallTimer); 
void avg(double sum[KERNELS], double data[KERNELS][iter]); 
double timer_start(int computeRank); 
double timer_end(double timerStart, int computeRank ); 
