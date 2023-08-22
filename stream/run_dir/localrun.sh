rm -rf LOCAL 
mkdir LOCAL 
cd LOCAL
EXE=/home/shrey/Coding/iocomp/stream/test
mpirun -n 2 ${EXE} --nx 1000 --ny 1000 --io 1 
