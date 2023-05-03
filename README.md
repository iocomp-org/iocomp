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

### Preprocessor flags for the iocomp src/Makefile 
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

## iocomp run commands 
Command line arguments
- nx : Dimension 1 of STREAM vector. 
- ny : Dimensions 2 of STREAM vector. nx * ny give the total size of array that is passed to STREAM. 
- io : I/O backend selection. 
    - 0 : MPIIO
    - 1 : HDF5 
    - 2 : ADIOS2_HDF5 
    - 3 : ADIOS2_BP4 
    - 3 : ADIOS2_BP5
- HT : Flag which when passed splits the communicators to have different operations on different processes. 
```
# for the default case with sequential compute and I/O operations 
./test --nx=<size of nx> --ny=<size of ny> --io=<I/O selection>

# for the HT case with compute and I/O operations on different MPI processes 
./test --HT --nx=<size of nx> --ny=<size of ny> --io=<I/O selection>
```

I/O selection is using 
