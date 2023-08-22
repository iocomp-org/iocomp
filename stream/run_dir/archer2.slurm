#!/bin/bash --login
#SBATCH --job-name=stream
#SBATCH --nodes=1
#SBATCH --tasks-per-node=128
#SBATCH --cpus-per-task=1
#SBATCH --time=00:10:00
#SBATCH --account=e609
#SBATCH --partition=standard 
#SBATCH --qos=lowpriority

export STREAM_DIR=$(cd ${SLURM_SUBMIT_DIR}/../../ && pwd) 

# GNU 
module swap PrgEnv-cray/8.0.0 PrgEnv-gnu
module load cmake 
module use /work/y07/shared/archer2-lmod/dev
module load xthi 
module swap craype-network-ofi craype-network-ucx 
module swap cray-mpich cray-mpich-ucx 
module load cray-hdf5-parallel
export IOCOMP_DIR=/work/e609/e609/shr203/opt/gnu/8.0.0/iocomp
export LD_LIBRARY_PATH=${IOCOMP_DIR}/lib:${LD_LIBRARY_PATH} 
export ADIOS2_DIR=/work/e609/e609/shr203/opt/gnu/8.0.0/ADIOS2
export LD_LIBRARY_PATH=${ADIOS2_DIR}/lib64:${LD_LIBRARY_PATH} 
export EXE=${STREAM_DIR}/stream/test # production run 

## for darshan runs 
#export DXT_ENABLE_IO_TRACE=1
#module load darshan

## for perftools, comment them out 
#module load perftools-base/21.02.0
#export EXE=${STREAM_DIR}/stream/test+pat # cray pat 

# Setup environment
export PPN=${SLURM_NTASKS_PER_NODE}
export OMP_NUM_THREADS=1
export CONFIG=${SLURM_SUBMIT_DIR}/config.xml 
export MAP=0

IOLAYERS=("MPIIO" "HDF5" "ADIOS2_HDF5" "ADIOS2_BP4" "ADIOS2_BP5") # assign IO layer array 
i=${SLURM_ARRAY_TASK_ID} 
HALF_CORES=$((${SLURM_NTASKS_PER_NODE}/2)) 
FULL_CORES=$((${SLURM_NTASKS_PER_NODE})) 
HALF_NODES=$((${SLURM_NNODES}/2))
TOTAL_RANKS=$(($SLURM_NNODES * $SLURM_NTASKS_PER_NODE))

#SIZE=$(( ${NX}*${NY}*8 / 2**20 )) # local size in MiB

echo "Job started " $(date +"%T") # start time

for IO in $(seq ${IO_START} ${IO_END})
do 
  export PARENT_DIR=${SLURM_SUBMIT_DIR}/${DIR}/${SLURM_NNODES}_${SLURM_NTASKS_PER_NODE}/${NX}/${NY}/${IOLAYERS[${IO}]}

  # Case 1 
  source ${SLURM_SUBMIT_DIR}/slurm_files/sequential.sh
  wait 
  # Case 2
	source ${SLURM_SUBMIT_DIR}/slurm_files/oversubscribe.sh 
	wait 
  # Case 3
  source ${SLURM_SUBMIT_DIR}/slurm_files/hyperthread.sh 
  wait 
  # Case 4
  source ${SLURM_SUBMIT_DIR}/slurm_files/consecutive.sh
	wait 

done 

echo $(module list) 

echo "Job ended " $(date +"%T") # end time 
