#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "iocomp.h"

void computeStep(double* iodata, struct iocomp_params *iocompParams)
{
	iodata = initData(iocompParams); 
	computeServer(iodata,iocompParams); 
} 

