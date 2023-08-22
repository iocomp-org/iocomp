# bash script to perform global scaling on stream 
# export SIZE variable to archer2 slurm script 

PPN=128 # tasks per node 

## Global scaling 
#GLOBAL_SIZE=4096 # size of array per core for 1 node jobs 
#for i in $(seq 0 3) # 1 till 16 nodes 
#do 
#
#  SIZE_LOCAL=$((${GLOBAL_SIZE}/(2**${i}))) # local size per node 
#  NUM_NODES=$((2**(2*${i})))
#  echo NODES ${NUM_NODES} SIZE  ${SIZE_LOCAL} 
#  sbatch --export=ALL,SIZE=${SIZE_LOCAL} --qos=lowpriority --nodes=${NUM_NODES} --ntasks-per-node=${PPN} --time=5:00:00  archer2.slurm 
#
#done 

### Weak scaling 
#
## fixed at 128 tasks per node
#PPN=128  
## array parameters 
#NX=4096
#NY=4096
## number of nodes, as power of 2s
#NODE_START=0
#NODE_END=4
## directory 
#DIR=v1.1.4/WEAK
## I/O layers range 
#IO_START=0
#IO_END=3
## time for job  
#TIMES=("5:00:00" "06:00:00" "08:00:00" "10:00:00" "10:00:00") 
#ARRAY=0
#
#echo $DIR 
## iterate through number of nodes 
#for i in $(seq ${NODE_START} ${NODE_END}) # 1 till 16 nodes 
#do 
#  TIME_VAR=${TIMES[${i}]} 
#  NUM_NODES=$((2**${i}))
#  echo NODES ${NUM_NODES} ARRAY SIZE ${NX} x ${NY}  TIME ${TIME_VAR} IO ${IO_START} to ${IO_END} 
#  # sbatch --export=ALL,SIZE=${SIZE_LOCAL},NX=${NX},NY=${NY},DIR=${DIR},IO_START=${IO_START},IO_END=${IO_END} --qos=lowpriority --nodes=${NUM_NODES} --ntasks-per-node=${PPN} --time=${TIME_VAR} --array=${ARRAY} archer2.slurm 
#
#done 


## TESTING
# fixed at 128 tasks per node
PPN=128  
NUM_NODES=32
# directory 
DIR=TIMER
# I/O layers range 
IO_START=0
IO_END=3
echo $DIR 
# iterate through number of elements 
for i in $(seq ${IO_START} ${IO_END}) # 1 till 16 nodes 
do 
  # array parameters 
  NX=4096
  NY=4096
  TIME_VAR=03:00:00
  echo NODES ${NUM_NODES} ARRAY SIZE ${NX} x ${NY}  TIME ${TIME_VAR} IO ${IO_START} to ${IO_END} 
  sbatch --export=ALL,SIZE=${SIZE_LOCAL},NX=${NX},NY=${NY},DIR=${DIR},IO_START=${i},IO_END=${i} --qos=lowpriority --nodes=${NUM_NODES} --ntasks-per-node=${PPN} --time=${TIME_VAR}  archer2.slurm 
done 

