#!/bin/bash --login
#SBATCH --job-name=xthi
#SBATCH --nodes=2
#SBATCH --tasks-per-node=256
#SBATCH --time=0:01:00
#SBATCH --account=e609
#SBATCH --partition=standard 
#SBATCH --qos=lowpriority

module use /work/y07/shared/archer2-lmod/dev
module load xthi 

# Setup environment
export OMP_NUM_THREADS=1
#export HALF_CORES=$((${SLURM_NTASKS_PER_NODE}/3)) 
export FULL_CORES=${SLURM_NTASKS_PER_NODE}
export HALF_CORES=$((${FULL_CORES}/2)) 

# Serial case
echo "** Sequential **"
#end=$((${HALF_CORES}-1))
#end=$(($FULL_CORES - 1)) 
#vals=($(seq 0 1 $(eval echo ${end})))
#bar=$(IFS=, ; echo "${vals[*]}")
#srun --cpu-bind=verbose --hint=nomultithread  --distribution=block:block --ntasks=${HALF_CORES} --cpu-bind=map_cpu:${bar[@]} xthi 
srun --cpu-bind=verbose --hint=nomultithread  --distribution=block:block --ntasks=${HALF_CORES} --nodes=1  xthi 

# Overcommit case 
echo "** Oversubscribe **"
end=$((${HALF_CORES}-1)) # for one node or less 
vals=($(seq 0 1 $(eval echo ${end})))
bar=$(IFS=, ; echo "${vals[*]}")
srun  --cpu-bind=verbose --hint=nomultithread --distribution=block:block --ntasks=${FULL_CORES} --cpu-bind=map_cpu:${bar[@]} --overcommit --nodes=1 xthi
#srun  --cpu-bind=verbose --hint=nomultithread --distribution=block:block --ntasks=${FULL_CORES} --nodes=1 --oversubscribe  xthi

# Hyperthreaded 
echo "** Hyperthread **"
### for intertwining hyperthreads with normal cores. 
#end=$((${HALF_CORES}-1))
#vals=($(seq 0 1 $(eval echo ${end})))
## seq 2 
#end=$((${HALF_CORES}+128-1))
#start=128
#vals_HT=($(seq $(eval echo ${start}) 1 $(eval echo ${end})))
#for i in `seq 0 $(($FULL_CORES-1))`;
#do 
#  if [ "$((i%2))" -eq 0 ];
#  then 
#    half_i=$(($i/2))
#    updated[$i]=$((vals[${half_i}])) 
#  else
#    half_i=$((($i-1)/2))
#    updated[$i]=$((vals_HT[${half_i}]))
#  fi 
#done 

##  for placing Hyperthread cores at end. 
end=$((${HALF_CORES}-1))
vals=($(seq 0 1 $(eval echo ${end})))
end=$((${HALF_CORES}+128-1))
start=128
vals_HT=($(seq $(eval echo ${start}) 1 $(eval echo ${end})))
updated=("${vals[@]}" "${vals_HT[@]}")
bar=$(IFS=, ; echo "${updated[*]}")
#srun  --cpu-bind=verbose --hint=multithread --distribution=block:block --ntasks=${FULL_CORES} --cpu-bind=map_cpu:${bar[@]}  xthi
srun  --cpu-bind=verbose --hint=multithread --distribution=block:block --ntasks=${FULL_CORES} --cpu-bind=map_cpu:${bar[@]} --nodes=1 xthi


## Consecutive cores 
echo "** Consecutive cores"
#end=$((${FULL_CORES}-1))
#vals=($(seq 0 1 $(eval echo ${end})))
#bar=$(IFS=, ; echo "${vals[*]}")
#srun --cpu-bind=verbose --hint=nomultithread  --distribution=block:block --ntasks=${FULL_CORES} --nodes=2 --cpu-bind=map_cpu:${bar[@]} xthi 
srun --cpu-bind=verbose --hint=nomultithread  --distribution=block:block --ntasks=${FULL_CORES} --nodes=2  xthi 



