#!/bin/bash --login
#SBATCH --job-name=str_test
#SBATCH --nodes=1
#SBATCH --tasks-per-node=128
#SBATCH --cpus-per-task=1
#SBATCH --time=00:10:00
#SBATCH --account=e609
#SBATCH --partition=standard 
#SBATCH --qos=lowpriority
##SBATCH --array=0-2

module swap PrgEnv-cray/8.0.0 PrgEnv-gnu
module load cmake 
module use /work/y07/shared/archer2-lmod/dev
module load xthi 
module swap craype-network-ofi craype-network-ucx 
module swap cray-mpich cray-mpich-ucx 
module load cray-hdf5-parallel
#module load arm/forge

# Setup environment
export PPN=${SLURM_NTASKS_PER_NODE}
export OMP_NUM_THREADS=1
export IOCOMP_DIR=/work/e609/e609/shr203/iocomp
export ADIOS2_DIR=/work/e609/e609/shr203/opt/gnu/8.0.0/ADIOS2
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${ADIOS2_DIR}/lib64:${IOCOMP_DIR}/lib
export CONFIG=${SLURM_SUBMIT_DIR}/config.xml 
export MAP=0
export STREAM_DIR=$(cd ${SLURM_SUBMIT_DIR}/../../ && pwd) 
export EXE=${STREAM_DIR}/stream/test

IOLAYERS=("MPIIO" "HDF5" "ADIOS2_HDF5" "ADIOS2_BP4" "ADIOS2_BP5") # assign IO layer array 
i=${SLURM_ARRAY_TASK_ID} 
HALF_CORES=$((${SLURM_NTASKS_PER_NODE}/2)) 
FULL_CORES=$((${SLURM_NTASKS_PER_NODE})) 
HALF_NODES=$((${SLURM_NNODES}/2))
TOTAL_RANKS=$(($SLURM_NNODES * $SLURM_NTASKS_PER_NODE))

NX=4096
NY=4096
echo "Job started " $(date +"%T") # start time

for IO in {0..0}
do 
  #export PARENT_DIR=${SLURM_SUBMIT_DIR}/v1.1.1/GLOBAL/LOOP100/WRITE10/${SLURM_NNODES}_${SLURM_NTASKS_PER_NODE}/${SIZE}/${IOLAYERS[${IO}]}
  export PARENT_DIR=${SLURM_SUBMIT_DIR}/v1.1.2/TEST/${SLURM_NNODES}_${SLURM_NTASKS_PER_NODE}/${SIZE}/${IOLAYERS[${IO}]}

#  # Case 1 
#  source ${SLURM_SUBMIT_DIR}/slurm_files/sequential.sh
#  wait 
#  # Case 2
#	source ${SLURM_SUBMIT_DIR}/slurm_files/oversubscribe.sh 
#	wait 
  # Case 3
  source ${SLURM_SUBMIT_DIR}/slurm_files/hyperthread.sh 
  wait 
#  # Case 4
#  source ${SLURM_SUBMIT_DIR}/slurm_files/consecutive.sh
#	wait 
#
done 

echo $(module list) 

echo "Job ended " $(date +"%T") # end time 
