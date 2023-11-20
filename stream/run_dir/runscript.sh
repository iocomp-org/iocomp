# bash script to perform global scaling on stream 
# export SIZE variable to archer2 slurm script 

# tasks per node
PPN=128

# array parameters 
NX=$((2**14)) # for strong scaling 
NY=$((2**11)) 
# NX=$((2**12)) # for weak scaling  
# NY=$((2**12)) 

# node start and end as power of 2s 
NODE_START=6
NODE_END=6

# I/O selection range 
IO_START=0
IO_END=3

# Job numbers for averaging 
ARRAY="0-2"

# time per job for custom time
#TIME="3:00:00"

# weak scaling script and directory
# DIR=OUTPUT/v2.0.0/WEAK/100COMPUTE
# source weakScaling.sh 

# directory for strong scaling 
DIR=OUTPUT/v2.0.0/STRONG/GLOBALSIZE_8GB/1COMPUTE
source strongScaling.sh 

