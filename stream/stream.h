#include "mpi.h"
#include "iocomp.h"

#define NDIM 2  // power to size
#define KERNELS 4
#define AVGLOOPCOUNT 20 // number of average cycles 
#define COMPLOOPCOUNT 1 // number of compute cycles
#define WRITE_FREQ 5 // frequency of writing 
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
	double compTimer[KERNELS][AVGLOOPCOUNT]; 
	double waitTimer[KERNELS][AVGLOOPCOUNT]; 
	double sendTimer[KERNELS][AVGLOOPCOUNT]; // num writes could be different to num compute
	// reduced arrays to obtain max values 
	double maxCompTimer[KERNELS][AVGLOOPCOUNT]; 
	double maxWaitTimer[KERNELS][AVGLOOPCOUNT]; 
	double maxSendTimer[KERNELS][AVGLOOPCOUNT]; 
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
void scale(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* b); 
void add(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* a, double* b); 
void triad(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c, double* a, double* b ); 
// waiting functions 
void copy_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k); 
void scale_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k); 
void add_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k); 
void triad_wait(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k); 
// sending functions 
void copy_send(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c); 
void scale_send(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* b); 
void add_send(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* c); 
void triad_send(struct iocomp_params *iocompParams, struct stream_params* streamParams, int k, double* a); 
//values check function 
//void test_vals(struct iocomp_params *iocompParams, struct stream_params* streamParams, double *testArray, char* KERNEL); 
//void checkArray(struct stream_params* streamParams, double *readData, double* testData); 

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
