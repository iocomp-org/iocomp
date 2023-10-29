# bash script to perform global scaling on stream 
# export SIZE variable to archer2 slurm script 

# tasks per node
PPN=128

# array parameters 
NX=$((2**11)) 
NY=$((2**11)) 

# node start and end as power of 2s 
NODE_START=0
NODE_END=0

# I/O selection range 
IO_START=0
IO_END=0

# Job numbers for averaging 
ARRAY="0"

# time per job for custom time
TIME="00:10:00"

# shared, HT or none? 
# FLAG="shared"
FLAG="HT"

# directory. Check if flag defined as HT or shared. 
# If not defined, then save under nosplit. 
if [ -n "$FLAG" ];
then 
  DIR=OUTPUT/v2.0.0/TESTING/${FLAG}
else
  DIR=OUTPUT/v2.0.0/TESTING/NOSPLIT
fi 

source weakScaling.sh 



# directory for strong scaling 
#DIR=v1.1.4/STRONG
#source strongScaling.sh 

