#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"
#define iter 5
#define KERNELS 4

// number of iterations for averaging the stream tests 
void computeStep(double* iodata, struct iocomp_params *iocompParams);
void stream(double* iodata, struct iocomp_params *iocompParams); 
void initData(double* iodata, struct iocomp_params *iocompParams);
void resultsOutput(double timer[4][iter], double totalTimer[4][iter],double waitTimer[4][iter], double wallTimer); 
void avg(double sum[KERNELS], double data[KERNELS][iter]); 
void timer_start(double* timerStart, int computeRank); 
double timer_end(double* timerStart, int computeRank ); 
