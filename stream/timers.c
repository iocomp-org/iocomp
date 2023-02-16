#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "stdio.h"
#include "mpi.h"
#include "stream.h"

double timer_start(int computeRank)
{
	double timerStart = 0.0; 	
	if (computeRank == 0) // timing will be measured by using ioRank = 0 
	{	
		timerStart = MPI_Wtime();
	}
	return(timerStart); 
}

double timer_end(double timerStart, int computeRank )
{	
	double timeDiff = 0.0; 
	if (computeRank == 0) // timing will be measured by using ioRank = 0 
	{	
		timeDiff = MPI_Wtime() - timerStart; 
	} 
	return(timeDiff); 
}

