#include "stdio.h"
#include "test.h"

bool checkData(int io, struct test_variables* testParams)
{
	bool ierr = 0; 
	testParams->adios2Engine[0] = "HDF5"; 
	testParams->adios2Engine[1] = "BP4"; 
	testParams->adios2Engine[2] = "BP5"; 

	switch(io){
		case 0: 
			ierr = mpiRead(testParams); 
			break; 

		case 1: 
			hdf5Read(testParams); 
			break; 

		case 2: case 3: case 4: 
			adios2Read(testParams, io); 
			break; 

		 default:
				break; 
	}
	return ierr; 
}
