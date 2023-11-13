# bash script to perform global scaling on stream 
# export SIZE variable to archer2 slurm script 

# tasks per node
PPN=128

# array parameters 
NX=$((2**14)) # for strong scaling 
# NX=$((2**11)) # for weak scaling  
NY=$((2**11)) 

# node start and end as power of 2s 
NODE_START=1
NODE_END=1

# I/O selection range 
IO_START=3
IO_END=3

# Job numbers for averaging 
ARRAY="0-2"

# time per job for custom time
TIME="3:00:00"

# weak scaling script and directory
#DIR=ARM_MAP/v2.0.0/MPICH/1000COMPUTE
# source weakScaling.sh 

# directory for strong scaling 
DIR=OUTPUT/v2.0.0/STRONG/GLOBALSIZE_8GB/100COMPUTE
source strongScaling.sh 

