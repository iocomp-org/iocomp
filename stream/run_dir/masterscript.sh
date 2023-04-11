# bash script to perform global scaling on stream 
# export SIZE variable to archer2 slurm script 

PPN=128 # tasks per node 
GLOBAL_SIZE=4096 # size of array per core for 1 node jobs 

for i in $(seq 0 3) # 1 till 16 nodes 
do 

  SIZE_LOCAL=$((${GLOBAL_SIZE}/(2**${i}))) # local size per node 
  NUM_NODES=$((2**(2*${i})))
  echo NODES ${NUM_NODES} SIZE  ${SIZE_LOCAL} 
  sbatch --export=ALL,SIZE=${SIZE_LOCAL} --qos=lowpriority --nodes=${NUM_NODES} --ntasks-per-node=${PPN} --time=5:00:00  archer2.slurm 

done 
