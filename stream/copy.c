#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "test.h"

void copy(iocomp_params* iocompParams, double* c, double* a)
{
		timerStart = timer_start(computeRank); // start timing 
		for(i = 0; i< iocompParams->localDataSize; i++)
		{
			c[i] = a[i]; 
			if(k>0){mpiWaitFlag[TRIAD] = dataSendTest(iocompParams,&requestArray[TRIAD]);} // test if TRIAD data got sent
		}
		timer[COPY][k] = timer_end(timerStart,computeRank); // computeTime for COPY 
		dataSend(c,iocompParams, &requestArray[COPY]); // send data off using dataSend
		totalTimer[COPY][k] = timer_end(timerStart,computeRank); // total time for COPY  
#ifndef NDEBUG
		for(i = 0; i< iocompParams->localDataSize; i++) { printf("%lf,",c[i]); }
		printf("After COPY\n"); 
#endif

		// wait for data from previous SCALE(B) to be sent 
		if(k>0)
		{
			if(mpiWaitFlag[SCALE] == 0 && iocompParams->hyperthreadFlag!= 0) {printf("SCALE data not sent \n");}  // test if HT flag is on
			timerStart = timer_start(computeRank); // start timing 
			dataWait(iocompParams,&requestArray[SCALE]);
			waitTimer[SCALE][k] = timer_end(timerStart,computeRank); // wait time for SCALE 
		} 
} 
