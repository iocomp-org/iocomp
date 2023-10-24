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

	// define window control integers 
#define WIN_DEACTIVATE 0 
#define WIN_TEST 1
#define WIN_ACTIVATE 2 
#define WIN_WAIT 3
#define WIN_FREE -1

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
		int numWin; 

		// for data_output function 
		double writeTime; 
		char ioLibs[ioLibCount][100]; 
		// select ioLib
		int ioLibNum;  

		// for io_libraries function 
		int NDIM; 
		size_t* localArray; 
		size_t* arrayStart; 
		size_t* globalArray; 
		size_t globalDataSize; 
		size_t localDataSize; 
		MPI_Datatype dataType; 

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
		// previously initialised counter
		int previousInit;  
		// previous element count
		int previousCount;  
		// previous element count
		int adios2Init;  

#ifdef PRINT_ORDERING
		// so that getPair messages are not printed more than once.
		int pairPrintCounter; 
#endif 

		// shared memory addition communicators 
		MPI_Comm newComm; 
		MPI_Comm ioComm;

		// group 
		MPI_Group group; 

		bool sharedFlag; 
#ifndef NDEBUG
		// file object for debug 
		char debugFile[100]; 
		FILE* debug; 
#endif 
		// sync access control flags 
		int* wintestflags; 	
		// shared MPI window array
		MPI_Win* winMap;  
		// Shared window pointers and MPI window thing 
		double** array; 
		// flag value from MPI test routines 
		int* flag;
		// filename for writing 
		char** writeFile; 
	}; 
	extern struct iocomp_params iocompParams; 

	void ioLibraries(double* iodata, struct iocomp_params *iocompParams, int windowNum); 
	void timing_int(double time_diff, double global_data_size, int irep, int MAXLOOP_AVGIO, int local_n, char* filename, MPI_Comm comm);
	// void dataSend(double* data, struct iocomp_params *iocompParams, MPI_Request *request, size_t localDataSize); // function to send data using MPI_Isend 
	void dataSend(double* data, struct iocomp_params *iocompParams, MPI_Request *request, size_t localDataSize); 
	void ioServer(struct iocomp_params *iocompParams); 
	int getPair(struct iocomp_params *iocompParams); 
	void compute_comm_create(int color, MPI_Comm splitComm, MPI_Comm *computeComm);
	void comm_split(struct iocomp_params *iocompParams, MPI_Comm comm); 
	void arrayParamsInit(struct iocomp_params *iocompParams, MPI_Comm comm ); 
	void highlowOrdering(struct iocomp_params *iocompParams); 
	MPI_Comm iocompInit(struct iocomp_params *iocompParams, MPI_Comm comm, bool FLAG, 
			int ioLibNum, int fullNode, bool sharedFlag, int numWin); 
	void ioServerInitialise(struct iocomp_params *iocompParams); // initialise ioServer if ioProcessor 
	void testData(struct iocomp_params *iocompParams, int testFlag); // test data structures with flag to switch on/off  
	void stopSend(struct iocomp_params *iocompParams); // ghost send function that signals MPI_Sends are stopping
	void dataWait(struct iocomp_params *iocompParams,MPI_Request *request, double* array, char* fileName); 
	int dataSendTest(struct iocomp_params *iocompParams,MPI_Request *request, double* array); // wrapper function to implement testing of send data IF HT flag is on 

	// checks 
	void malloc_check(double* test); 
	void free_check(double* test); 

	// delete files and directories  
	// int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf); 
	void deleteFiles(struct iocomp_params* iocompParams, int windowNum); 
	void sendFileName(struct iocomp_params *iocompParams, char* fileName); 

	/*
	 * IO libraries write and read files 
	 */ 
	void mpiiowrite(double* iodata, struct iocomp_params *iocompParams, int windowNum);  
	void mpiRead(double *readData, char* FILENAME, struct iocomp_params *iocompParams); 
	void readBack(struct iocomp_params *iocompParams);  
#ifndef NOHDF5 
	void phdf5write(double* iodata, struct iocomp_params *iocompParams, int windowNum);  
	void hdf5Read(struct iocomp_params *iocompParams, double* iodata_test, char* fileName); 
	void phdf5Read(double *readData, char* fileName, struct iocomp_params *iocompParams); 
#endif 
#ifndef NOADIOS2
	void adioswrite(double* iodata, struct iocomp_params *iocompParams, int windowNum); 
	void adios2Read(double* iodata, char* FILENAME, struct iocomp_params *iocompParams); 
#endif 

	// Shared memory additions 
	void ioServer_shared(struct iocomp_params *iocompParams); 
	void fileWrite(struct iocomp_params *iocompParams, double* iodata, int* loopCounter, int windowNum); 
	void fileNameInit(struct iocomp_params *iocompParams, char filenames[iocompParams->numWin][100]); 
	void winInits(struct iocomp_params *iocompParams, int localdatasize); 
	void dataSendEnd(struct iocomp_params *iocompParams, double* array); 
	void dataSendStart(struct iocomp_params *iocompParams, double* array); 
	void dataSendInfo(struct iocomp_params *iocompParams); 
	void winActivateInfo(struct iocomp_params *iocompParams, double* array); 
	void winTestInfo(struct iocomp_params *iocompParams, double* array); 
	void winFreeInfo(struct iocomp_params *iocompParams, double* array); 
	void dataSendComplete(struct iocomp_params *iocompParams, double* array); 
	void winWaitInfo(struct iocomp_params *iocompParams, double* array); 
	void ioServer_sharedAllocate(struct iocomp_params *iocompParams); 
	void preDataSend(struct iocomp_params *iocompParams, double* array, char* fileName); 
	void winFree(struct iocomp_params *iocompParams, int i); 
	void winPost(struct iocomp_params *iocompParams, int i); 
	void winTest(struct iocomp_params *iocompParams, int i); 
	void winWait(struct iocomp_params *iocompParams, int i); 
	void commSplit_shared(struct iocomp_params *iocompParams); 
	void winFinalise(struct iocomp_params *iocompParams); 
	void getFileName(struct iocomp_params *iocompParams, int i); 

	// debug file 
	void initDebugFile(struct iocomp_params *iocompParams, int globalRank); 

#ifdef __cplusplus
}
#endif 
#endif 
