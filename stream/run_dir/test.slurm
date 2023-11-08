#!/bin/bash --login
#SBATCH --job-name=stream
#SBATCH --nodes=1
#SBATCH --tasks-per-node=128
#SBATCH --cpus-per-task=1
#SBATCH --time=00:05:00
#SBATCH --account=e609
#SBATCH --partition=standard 
#SBATCH --qos=standard

export STREAM_DIR=$(cd ${SLURM_SUBMIT_DIR}/../../ && pwd) 

# different module loads. Swap cray env to gnu env 
module swap PrgEnv-cray PrgEnv-gnu
module use /work/y07/shared/archer2-lmod/dev
module load cray-hdf5-parallel
module load gdb4hpc/4.14.6

# different directories and their paths
export IOCOMP_DIR=/work/e609/e609/shr203/opt/gnu/8.0.0/iocomp/2.0.0
export ADIOS2_DIR=/work/e609/e609/shr203/opt/gnu/8.0.0/ADIOS2
export LD_LIBRARY_PATH=${ADIOS2_DIR}/lib64:${IOCOMP_DIR}/lib:${LD_LIBRARY_PATH} 

# set file dirs for executable and make files that will be copied to rundir 
# export TEST_EXE=${STREAM_DIR}/test/test # test script
TEST_EXE=/mnt/lustre/a2fs-work3/work/e609/e609/shr203/iocomp/test/test
export CONFIG=${SLURM_SUBMIT_DIR}/config.xml 

# Ensure the cpus-per-task option is propagated to srun commands
export SRUN_CPUS_PER_TASK=$SLURM_CPUS_PER_TASK

# flag for archer2 runs 
export FI_OFI_RXM_SAR_LIMIT=64K

# avg jobs directories
iter=${SLURM_ARRAY_TASK_ID}

# Setup environment
export PPN=${SLURM_NTASKS_PER_NODE}
export OMP_NUM_THREADS=1

IOLAYERS=("MPIIO" "HDF5" "ADIOS2_HDF5" "ADIOS2_BP4" "ADIOS2_BP5") # assign IO layer array 
SIZE=$(( ${NX}*${NY}*8 / 2**20 )) # local size in MiB

echo "Job started " $(date +"%T") "size " ${SIZE} MiB # start time

for IO in $(seq ${IO_START} ${IO_END})
do 
  export CASE=Consecutive
  export RUNDIR=${SLURM_SUBMIT_DIR}/${DIR}/${SLURM_NNODES}_${SLURM_NTASKS_PER_NODE}/${SIZE}MiB/${IOLAYERS[${IO}]}/${CASE}/${iter}
  echo "Testing in directory:" ; echo ${RUNDIR}; cd ${RUNDIR} 
  srun ${TEST_EXE} --nx ${NX} --ny ${NY} --io ${IO} > testing.out 
done 

echo $(module list) 

echo "Job ended " $(date +"%T") # end time 
