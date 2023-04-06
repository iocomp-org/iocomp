#include "stdio.h"
#include "test.h"

bool checkData(int io, struct test_variables* testParams)
{
	bool ierr = 0; 
	switch(io){
		case 0: 
			ierr = mpiRead(testParams); 
			break; 

		case 1: 
			hdf5Read(testParams); 
			break; 

//		case 2: 
//			adios2Read(testParams); 
//			break; 

//		case 3: 
//			mpiRead(testParams); 
//			break; 
//
//		case 4: 
//			mpiRead(testParams); 
//			break; 
		 default:
				break; 
	}
	return ierr; 
}
