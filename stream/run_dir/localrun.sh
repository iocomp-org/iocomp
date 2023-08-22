rm -rf LOCAL 
mkdir LOCAL 
cd LOCAL
EXE=/home/shrey/Coding/iocomp/stream/test
mpirun -n 2 ${EXE} --nx 10 --ny 10 --io 1 
