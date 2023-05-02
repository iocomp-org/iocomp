#ifndef _IOCOMP_H_
#define _IOCOMP_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mpi.h"
#include <stdbool.h>
#include <stdio.h> 
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>  
#ifndef NOADIOS2 
#include <adios2_c.h>
#include <adios2/c/adios2_c_types.h>
#endif 

#define ioColour 0
#define compColour 1 
#define ioLibCount 5
#define NUM_DIM 2

	/*
	 * Header file for declaring the error_report_fn and macro error_report which simplifies calling.
	 */
#ifndef _ERROR_REPORT_H_
#define _ERROR_REPORT_H_


	/*
	 * error_report macro.
	 * Takes an error code as input, expanding to call the error_report_fn with the line number and file
	 * name values defined by the __LINE___ and __FILE__ macros.
	 */
#define mpi_error_check(ierr) error_report_fn(ierr, __LINE__, __FILE__)

	void error_report_fn(int ierr, int line_no, char *file_name);

#endif

#ifndef NDEBUG 
#define VERBOSE_1(rank, ...) if(rank==0) fprintf(stderr, __VA_ARGS__)
#define VERBOSE_2(output_file, ...) fprintf(output_file, __VA_ARGS__)
#endif 

	/*
	 * structure passes around to most functions in program 
	 */ 
	struct iocomp_params 
	{
		// setting up of interComms 
		MPI_Comm globalComm; 
		MPI_Comm ioServerComm; 
		MPI_Comm compServerComm; 
		MPI_Comm interComm; 
		MPI_Comm cartcomm; 

		// process parameters 
		bool hyperthreadFlag;  
		int colour; 
		int ioServerRank; 
		int compServerRank;
		int ioServerSize; 
		int compServerSize;
		int NODESIZE; 

		// for data_output function 
		double writeTime; 
		char ioLibs[ioLibCount][100]; 
		int ioLibNum; // select ioLib 

		// for io_libraries function 
		int NDIM; 
		size_t* localArray; 
		size_t* arrayStart; 
		size_t* globalArray; 
		size_t globalDataSize; 
		size_t localDataSize; 
		MPI_Datatype dataType; 

		// filenames 
		char *FILENAMES[5]; 
		// adios2 config file list 
		char *ADIOS2_IOENGINES[3]; 
#ifndef NOADIOS2
		// adios2 object 
		adios2_adios *adios;  
		// adios2 io object 
		adios2_io* io; 
		// adios2 variable object 
		adios2_variable *var_iodata; 
#endif 
		// initialisation flags 
		int previousInit; // previously initialised counter 
		int previousCount; // previous element count 
		int adios2Init; // previous element count 

#ifndef NDEBUG
		char DEBUG_FILE[100]; // filename for debug 
		FILE* debug_out; // output file per rank 
#endif 

#ifdef PRINT_ORDERING
		int pairPrintCounter; // so that getPair messages are not printed more than once.
#endif 
	}; 
	extern struct iocomp_params iocompParams; 

	void ioLibraries(double* iodata, struct iocomp_params *iocompParams); 
	void timing_int(double time_diff, double global_data_size, int irep, int MAXLOOP_AVGIO, int local_n, char* filename, MPI_Comm comm);
	void dataSend(double* data, struct iocomp_params *iocompParams, MPI_Request *request, size_t localDataSize); // function to send data using MPI_Isend 
	void ioServer(struct iocomp_params *iocompParams); 
	int getPair(struct iocomp_params *iocompParams); 
	void compute_comm_create(int color, MPI_Comm splitComm, MPI_Comm *computeComm);
	void comm_split(struct iocomp_params *iocompParams, MPI_Comm comm); 
	void arrayParamsInit(struct iocomp_params *iocompParams, MPI_Comm comm ); 
	void highlowOrdering(struct iocomp_params *iocompParams); 
	MPI_Comm iocompInit(struct iocomp_params *iocompParams, MPI_Comm comm, bool FLAG, int ioLibNum, int NODESIZE); // initialises the library 
	void ioServerInitialise(struct iocomp_params *iocompParams); // initialise ioServer if ioProcessor 
	void testData(struct iocomp_params *iocompParams, int testFlag); // test data structures with flag to switch on/off  
	void stopSend(struct iocomp_params *iocompParams); // ghost send function that signals MPI_Sends are stopping
	void dataWait(struct iocomp_params *iocompParams, MPI_Request *request); // wrapper function to implement waiting of data IF HT flag is activated 
	int dataSendTest(struct iocomp_params *iocompParams,MPI_Request *request); // wrapper function to implement testing of send data IF HT flag is on 

	// checks 
	void malloc_check(double* test); 
	void free_check(double* test); 

	// delete files and directories  
	// int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf); 
	void deleteFiles(struct iocomp_params* iocompParams); 

	/*
	 * IO libraries write and read files 
	 */ 
	void mpiiowrite(double* iodata, struct iocomp_params *iocompParams);  
	void mpiRead(struct iocomp_params *iocompParams, double* iodata_test); 
	void readBack(struct iocomp_params *iocompParams);  
#ifndef NOHDF5 
	void phdf5write(double* iodata, struct iocomp_params *iocompParams);  
	void hdf5Read(struct iocomp_params *iocompParams, double* iodata_test); 
#endif 
#ifndef NOADIOS2
	void adioswrite(double* iodata, struct iocomp_params *iocompParams); 
	void adios2Read(struct iocomp_params *iocompParams, double* iodata_test); 
#endif 

#ifdef __cplusplus
}
#endif 
#endif 
