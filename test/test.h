#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"
#define iter 5

// number of iterations for averaging the stream tests 
void computeStep(double* iodata, struct iocomp_params *iocompParams);
void stream(double* iodata, struct iocomp_params *iocompParams); 
void initData(double* iodata, struct iocomp_params *iocompParams);
void resultsOutput(double timer[4][iter], double totalTimer[4][iter]); 
