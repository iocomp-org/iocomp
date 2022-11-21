#!/bin/bash --login
#SBATCH --job-name=xthi
#SBATCH --nodes=1
#SBATCH --tasks-per-node=128
#SBATCH --time=0:10:00
#SBATCH --account=e609
#SBATCH --partition=standard 
#SBATCH --qos=lowpriority

module use /work/y07/shared/archer2-lmod/dev
module load xthi 

# Setup environment
export OMP_NUM_THREADS=1
#export HALF_CORES=$((${SLURM_NTASKS_PER_NODE}/3)) 
#export FULL_CORES=${SLURM_NTASKS_PER_NODE}

export FULL_CORES=8
export HALF_CORES=$((${FULL_CORES}/2)) 

# Serial case
echo "** Serial"
end=$((${HALF_CORES}-1))
vals=($(seq 0 1 $(eval echo ${end})))
bar=$(IFS=, ; echo "${vals[*]}")
srun --cpu-bind=verbose --hint=nomultithread  --distribution=block:block --ntasks=${HALF_CORES} --cpu-bind=map_cpu:${bar[@]} xthi 

# Overcommit case 
echo "** Overcommit"
end=$((${HALF_CORES}-1))
vals=($(seq 0 1 $(eval echo ${end})))
bar=$(IFS=, ; echo "${vals[*]}")
#srun --overcommit --cpu-bind=verbose --hint=nomultithread --distribution=block:block --ntasks=${FULL_CORES} --sockets-per-node=1 --cores-per-socket=${HALF_CORES}  --threads-per-core=2  xthi
#srun  --cpu-bind=verbose --hint=nomultithread --distribution=block:block --ntasks=${FULL_CORES} --ntasks-per-core=2 --overcommit  xthi
#srun  --cpu-bind=verbose --hint=nomultithread --distribution=block:block --ntasks=${FULL_CORES} --cpu-bind=map_cpu:$(eval echo ${bar[@]}) --overcommit  xthi
srun  --cpu-bind=verbose --hint=nomultithread --distribution=block:block --ntasks=${FULL_CORES} --cpu-bind=map_cpu:${bar[@]} --overcommit  xthi

# Hyperthreaded 
echo "** Hyperthread"
end=$((${HALF_CORES}-1))
vals=($(seq 0 1 $(eval echo ${end})))
# seq 2 
end=$((${HALF_CORES}+128-1))
start=128
vals_HT=($(seq $(eval echo ${start}) 1 $(eval echo ${end})))
for i in `seq 0 $(($FULL_CORES-1))`;
do 
  if [ "$((i%2))" -eq 0 ];
  then 
    half_i=$(($i/2))
    updated[$i]=$((vals[${half_i}])) 
  else
    half_i=$((($i-1)/2))
    updated[$i]=$((vals_HT[${half_i}]))
  fi 
done 
echo "${updated[*]}" 

bar=$(IFS=, ; echo "${updated[*]}")
srun  --cpu-bind=verbose --hint=multithread --distribution=block:block --ntasks=${FULL_CORES} --cpu-bind=map_cpu:${bar[@]}  xthi

## Consecutive cores 
echo "** Consecutive cores"
end=$((${FULL_CORES}-1))
vals=($(seq 0 1 $(eval echo ${end})))
bar=$(IFS=, ; echo "${vals[*]}")
srun --cpu-bind=verbose --hint=nomultithread  --distribution=block:block --ntasks=${FULL_CORES} --cpu-bind=map_cpu:${bar[@]} xthi 



