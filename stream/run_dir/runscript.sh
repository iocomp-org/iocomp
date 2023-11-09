# bash script to perform global scaling on stream 
# export SIZE variable to archer2 slurm script 

# tasks per node
PPN=128

# array parameters 
NX=$((2**13)) 
NY=$((2**10)) 

# node start and end as power of 2s 
NODE_START=4
NODE_END=5

# I/O selection range 
IO_START=0
IO_END=3

# Job numbers for averaging 
ARRAY="0-2"

# time per job for custom time
TIME="2:00:00"


# weak scaling script and directory
# DIR=OUTPUT/v2.0.0/WEAK/100COMPUTE
# source weakScaling.sh 

# directory for strong scaling 
DIR=OUTPUT/v2.0.0/STRONG
source strongScaling.sh 

