#!/bin/bash --login
#SBATCH --job-name=xthi
#SBATCH --nodes=1
#SBATCH --tasks-per-node=2
#SBATCH --cpus-per-task=1
#SBATCH --time=0:10:00
#SBATCH --account=e609
#SBATCH --partition=standard 
#SBATCH --qos=lowpriority

module use /work/y07/shared/archer2-lmod/dev
module load xthi 

# Setup environment
export OMP_NUM_THREADS=1

# Serial case
echo "Serial"
srun --hint=nomultithread --distribution=block:block --ntasks=1 xthi

# Overcommit case 
echo "Commit"
srun --overcommit --ntasks-per-node=2  xthi

# Hyperthreaded 
echo "Hyperthread"
srun --hint=multithread --distribution=block:block xthi

# Consecutive cores 
echo "Consecutive cores"
srun --ntasks=2 --cpus-per-task=1 --distribution=block:block --hint=nomultithread xthi 



