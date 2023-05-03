# ioComp

Library to compare performance from different I/O libraries with the option to switch between asynchronous and direct synchronous I/O. 
Different back-ends include MPIIO, HDF5, ADIOS2 HDF5 and ADIOS2 BP4, BP5

## Installation for iocomp library 
```
git clone <iocomp repository>
cd iocomp 
cd src 
mkdir Object_files
make DIR=</path/to/installation/dir>
# if only MPI compilation required
make DIR=</path/to/installation/dir> NOADIOS2=1 NOHDF5=1
``` 

### Preprocessor flags 
- NDELETE : add this flag to not delete the files after finishing writing 
- NDEBUG : add this flag to not print out error messages 
- IOCOMP_TIMERS : add this flag to print out I/O bandwidths after each step of writing is completed 
- PRINT_ORDERING : add this flag to print out ordering between the pairs 
- READBACK : add this flag to print out the file that was just written by the selected I/O back-end  

## Installation for iocomp STREAM implementation 
To test the library with a STREAM implementation: 
```
cd iocomp 
cd stream 
mkdir Object_files
make 
```

