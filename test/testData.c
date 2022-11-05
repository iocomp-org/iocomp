#include <stdlib.h>
#include "stdio.h"
#include "test.h"
#define filename "mpiio.dat" 
// test data after reading from dat file 
void testData(struct iocomp_params *iocompParams, int testFlag)
{
    if(testFlag)
		{
			FILE *fp;
			size_t size_char, test_fread;
			size_char = sizeof(double); 
			double* iodata_test = NULL;
			int total_size = iocompParams->globalDataSize; 
			/* For comparison with benchio_fortran */
			iodata_test = (double*)malloc(total_size * sizeof(double));

			fp = fopen(filename, "r");
			if (fp==NULL)
			{
					return; 
			}
				
			test_fread = fread(iodata_test, size_char, total_size, fp);

			if(test_fread != total_size) 
			{
				printf("total read elements are not equal to the global size \n");
				return;
			}

			for ( int i = 1; i < total_size - 1; i++)
			{
				if( (iodata_test[i] != iodata_test[i+1] - 1) || (iodata_test[i] != iodata_test[i-1] + 1) ) 
				{
					printf("data test failed, at element %i \n", i); 
					return; 
				} 
			}
			
			fclose(fp);
			free(iodata_test);
			iodata_test = NULL;
		} 

}
