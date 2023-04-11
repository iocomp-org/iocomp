#include "mpi.h"
#include "iocomp.h"

#define NDIM 2  // power to size
#define KERNELS 4
#define LOOPCOUNT 100 // number of compute steps 
#define MAXWRITES 10 // max number of writes
#define COPY		0
#define SCALE		1
#define ADD			2
#define TRIAD		3
#define NODESIZE 128


// number of iterations for averaging the stream tests 

struct stream_params{
	int LOOP_COUNT; 
	double timer_start; 
	// timer arrays
	double compTimer[KERNELS][LOOPCOUNT]; 
	double waitTimer[KERNELS][LOOPCOUNT]; 
	double sendTimer[KERNELS][MAXWRITES]; // num writes could be different to num compute
	// reduced arrays to obtain max values 
	double maxCompTimer[KERNELS][LOOPCOUNT]; 
	double maxWaitTimer[KERNELS][LOOPCOUNT]; 
	double maxSendTimer[KERNELS][MAXWRITES]; 
	// average of those reduced arrays
	double avgCompTimer[KERNELS]; 
	double avgWaitTimer[KERNELS]; 
	double avgSendTimer[KERNELS]; 
	// wall time	
	double wallTimer; 
	double maxWallTimer; 
	// sizes
	size_t localDataSize; 
	size_t globalDataSize; 
	MPI_Request requestArray[KERNELS]; 
	int mpiWaitFlag[KERNELS]; 
	char* fullResults_filename[KERNELS]; 
	int writeFreq; // compute steps per writing
	int numWrites; // max number of writes  
}; 
extern struct stream_params streamParams; 
// stream kernel functions 
void copy(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* a); 
void copy_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k); 
void add(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* a, double* b); 
void add_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k); 
void scale(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* b); 
void scale_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k); 
void triad(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* a, double* b ); 
void triad_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k); 

// others. 
void resultsOutput(struct stream_params* streamParams, MPI_Comm comm); 
void reduceResults(struct stream_params* streamParams, MPI_Comm comm); 
void fullResultsOutput(struct stream_params* streamParams); 
double* init(struct stream_params* streamParams, double* a); 
void computeStep(struct iocomp_params *iocompParams, struct stream_params* streamParams, MPI_Comm comm);
void stream(double* iodata, struct iocomp_params *iocompParams, struct stream_params* streamParams, MPI_Comm comm); 
void initData(double* iodata, struct iocomp_params *iocompParams);
void averages(struct stream_params* streamParams); 
double timer_start(int computeRank); 
double timer_end(double timerStart, int computeRank ); 
