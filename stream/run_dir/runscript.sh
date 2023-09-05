# bash script to perform global scaling on stream 
# export SIZE variable to archer2 slurm script 

# tasks per node
PPN=128

# array parameters 
NX=$((2**12)) 
NY=$((2**12)) 

# node start and end as power of 2s 
NODE_START=5
NODE_END=5

# I/O selection range 
IO_START=3
IO_END=3

# Job numbers for averaging 
ARRAY="0-2"

# time per job for custom time
TIME="04:00:00"

# directory for weak scaling 
DIR=OUTPUT/v1.1.4/COMPARISON
source weakScaling.sh 


# directory for strong scaling 
#DIR=v1.1.4/STRONG
#source strongScaling.sh 

