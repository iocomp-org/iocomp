#include "../include/iocomp.h"
void commSplit_shared(struct iocomp_params *iocompParams)
{
	/*
	 * Assuming IO process and Compute Process are mapped to physical and SMT cores
	 * if size = 10 then IO rank would be 5,6,..9
	 * and compute rank would be 0,1,..4
	 * Assign similar colours to corresponding I/O and compute Process
	 * i.e. rank 0 and rank 5 would have same colour and then same MPI
	 * Communicator 
	 */  
	// colour = globalRank%(globalSize/2); // IO rank and comp rank have same colour
	int globalRank, colour; 
	MPI_Comm_rank(iocompParams->globalComm, &globalRank); 
	colour = (int)globalRank/2; // IO rank and comp rank have same colour
	int ierr = MPI_Comm_split(MPI_COMM_WORLD, colour, globalRank, &iocompParams->newComm); 
	mpi_error_check(ierr); 
#ifndef NDEBUG
	fprintf(iocompParams->debug,"iocompInit -> shared flag true, MPI comm split into pairs of compute and IO server \n"); 
#endif

	int newRank; 
	ierr = MPI_Comm_rank(iocompParams->newComm,&newRank); 
	mpi_error_check(ierr); 

	// divide MPI comm world into compute or I/O server based on newRank which is
	// either 0 or 1 
	// assign compute and I/O rank 
	int computeRank, ioRank, computeSize, ioSize;

	if(newRank == 0)
	{
		colour = 0; 
		MPI_Comm_split(MPI_COMM_WORLD, colour, globalRank, &iocompParams->compServerComm);
		MPI_Comm_rank(iocompParams->compServerComm, &computeRank); 
		MPI_Comm_size(iocompParams->compServerComm, &computeSize); 
		if(!computeRank)
		{
			printf("Compute ranks have size %i \n", computeSize); 
		}
	}
	else
	{
		colour = 1; 
		MPI_Comm_split(MPI_COMM_WORLD, colour, globalRank, &iocompParams->ioServerComm);
		MPI_Comm_rank(iocompParams->ioServerComm, &ioRank); 
		MPI_Comm_size(iocompParams->ioServerComm, &ioSize); 
		if(!ioRank)
		{
			printf("IO ranks have size %i \n", ioSize); 
		}
	}

} 

