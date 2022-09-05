#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h> 
#include "iocomp.h"
#define output "output.csv"


extern struct iotime_stats_int{
   double start; 
   double end; 
   double avgtime;
   double maxtime; 
   double mintime;  
   double iorate; 
   double min_iorate; 
   double max_iorate;
   double avgiorate; 
   double std_error;  
} *time_stats_int; 

void timing_int(double time_diff, double global_data_size, int irep, int MAXLOOP_AVGIO, int local_n, char* filename, MPI_Comm comm)
{

	struct iotime_stats_int timer_stats;

	int test; 

	FILE* out;

	int size, rank; 

	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

#ifndef NDEBUG
	printf("remove filename \n");
#endif
	test = remove(output);
	if (test != 0)
	{
#ifndef NDEBUG
		printf("Cant remove %s \n", output);
#endif 
	}
	out = fopen(output, "w+");
	if (out == NULL)
	{
		printf("Error: No output file\n");
		exit(1);
	}



	fprintf(out, "IOLayer,LocalSize(GB),GlobalSize(GB),Local_N,Global_N,MPIRanks,AvgTime(s), AvgIORate(GB / s), MinIORate(GB / s), MaxIORate(GB / s), StdError\n"); //headers for output csv 


	timer_stats.avgtime = 0;
	timer_stats.avgiorate = 0;
	timer_stats.maxtime = 0;
	timer_stats.mintime = 0;
	timer_stats.max_iorate = 0;
	timer_stats.min_iorate = 0;

	double mtime[MAXLOOP_AVGIO]; 
	mtime[irep] = time_diff;                                          // calculate time taken 

	if (irep == 0) //first element 
	{
		timer_stats.mintime = timer_stats.maxtime = mtime[irep];
	}
	if (mtime[irep] <= timer_stats.mintime)                                            // mintime and maxtime 
	{
		timer_stats.mintime = mtime[irep];
	}
	else if (mtime[irep] >= timer_stats.maxtime)
	{
		timer_stats.maxtime = mtime[irep];
	}
	// STRONG SCALING
	timer_stats.iorate = global_data_size / mtime[irep];                                      //iorate in GB/s        

	timer_stats.min_iorate = global_data_size / timer_stats.maxtime;
	timer_stats.max_iorate = global_data_size / timer_stats.mintime;
	timer_stats.avgtime += mtime[irep] / (double)MAXLOOP_AVGIO;                       // average over MAXLOOP_AVGIOle
	timer_stats.avgiorate += timer_stats.iorate / (double)MAXLOOP_AVGIO;

	if (irep == MAXLOOP_AVGIO - 1) // has the averaging loop reached the end of its iteration? 
	{
		timer_stats.std_error = 0;
		for (int k = 0; k < MAXLOOP_AVGIO; k++)
		{
			timer_stats.std_error += pow((global_data_size / mtime[k] - timer_stats.avgiorate), 2);
		}
		timer_stats.std_error = sqrt(timer_stats.std_error / (MAXLOOP_AVGIO));
	}

	fprintf(out, "%s,%e,%e,%i,%i,%i,%e,%e,%e,%e,%e\n", filename, global_data_size/size, global_data_size, 
			local_n, local_n*size, size, timer_stats.avgtime, timer_stats.avgiorate, timer_stats.min_iorate,
			timer_stats.max_iorate, timer_stats.std_error);


}
