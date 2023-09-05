#!/bin/bash --login
#SBATCH --job-name=map
#SBATCH --nodes=1
#SBATCH --tasks-per-node=16
#SBATCH --cpus-per-task=1
#SBATCH --time=00:03:00
#SBATCH --account=e609
#SBATCH --partition=standard 
#SBATCH --qos=standard

module swap PrgEnv-cray/8.0.0 PrgEnv-gnu
module load cmake 
module use /work/y07/shared/archer2-lmod/dev
module load xthi 
module swap craype-network-ofi craype-network-ucx 
module swap cray-mpich cray-mpich-ucx 
module load cray-hdf5-parallel
module load arm/forge

# Setup environment
export PPN=${SLURM_NTASKS_PER_NODE}
export OMP_NUM_THREADS=1
export IOCOMP_DIR=/work/e609/e609/shr203/iocomp
export ADIOS2_DIR=/work/e609/e609/shr203/opt/gnu/8.0.0/ADIOS2
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${ADIOS2_DIR}/lib64:${IOCOMP_DIR}/lib
export EXE=${IOCOMP_DIR}/stream/test
export CONFIG=${SLURM_SUBMIT_DIR}/config.xml 

# Make new directory 
IOLAYERS=("MPIIO" "HDF5" "ADIOS2_HDF5" "ADIOS2_BP4" "ADIOS2_BP5") # assign IO layer array 
SIZE=4096
i=${SLURM_ARRAY_TASK_ID} 
HALF_CORES=$((${SLURM_NTASKS_PER_NODE}/2)) 
FULL_CORES=$((${SLURM_NTASKS_PER_NODE})) 
HALF_NODES=$((${SLURM_NNODES}/2))
TOTAL_RANKS=$(($SLURM_NNODES * $SLURM_NTASKS_PER_NODE))

MAP=1 # MAP variable, 0 for srun commands, 1 for map commands 

echo "Job started " $(date +"%T") # start time

for IO in {0..0}
do 
  export PARENT_DIR=${SLURM_SUBMIT_DIR}/ARM_MAP/v1.1/${SLURM_NNODES}_${SLURM_NTASKS_PER_NODE}/${SIZE}/${IOLAYERS[${IO}]}

  # Case 1 
#  source ${SLURM_SUBMIT_DIR}/slurm_files/sequential.sh
#  wait 
#  # Case 2
#	source ${SLURM_SUBMIT_DIR}/slurm_files/oversubscribe.sh 
#  wait 
#  # Case 3
# 	source ${SLURM_SUBMIT_DIR}/slurm_files/hyperthread.sh 
#  wait 
#  # Case 4
  source ${SLURM_SUBMIT_DIR}/slurm_files/consecutive.sh
#  wait 

done 

echo "Job ended " $(date +"%T") # end time 
echo $(module list) 
