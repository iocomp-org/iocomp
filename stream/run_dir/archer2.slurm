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
module swap craype-network-ofi craype-network-ucx 
module swap cray-mpich cray-mpich-ucx 
module load cray-hdf5-parallel

# different directories and their paths
export IOCOMP_DIR=/work/e609/e609/shr203/opt/gnu/8.0.0/iocomp/2.0.0
export LD_LIBRARY_PATH=${IOCOMP_DIR}/lib:${LD_LIBRARY_PATH} 
export ADIOS2_DIR=/work/e609/e609/shr203/opt/gnu/8.0.0/ADIOS2
export LD_LIBRARY_PATH=${ADIOS2_DIR}/lib64:${LD_LIBRARY_PATH} 

# set file dirs for executable and make files that will be copied to rundir 
export EXE=${STREAM_DIR}/stream/test # production run 
export TEST_EXE=${STREAM_DIR}/test/test # test script
export IOCOMP_MAKEOUTPUT=${STREAM_DIR}/src/iocomp_make.out # production run 
export STREAM_MAKEOUTPUT=${STREAM_DIR}/stream/stream_make.out # production run 
export CONFIG=${SLURM_SUBMIT_DIR}/config.xml 

# Ensure the cpus-per-task option is propagated to srun commands
# export SRUN_CPUS_PER_TASK=$SLURM_CPUS_PER_TASK

# avg jobs directories
iter=${SLURM_ARRAY_TASK_ID}

# flag for archer2 runs 
export FI_OFI_RXM_SAR_LIMIT=64K

## for darshan runs 
#export DXT_ENABLE_IO_TRACE=1
#module load darshan

## for perftools, comment them out 
#module load perftools-base/21.02.0
#export EXE=${STREAM_DIR}/stream/test+pat # cray pat 

# Setup environment
export PPN=${SLURM_NTASKS_PER_NODE}
export OMP_NUM_THREADS=1

# for ARM MAP 
# module load arm/forge 
export MAP=0

IOLAYERS=("MPIIO" "HDF5" "ADIOS2_HDF5" "ADIOS2_BP4" "ADIOS2_BP5") # assign IO layer array 
SIZE=$(( ${NX}*${NY}*8 / 2**20 )) # local size in MiB

echo "Job started " $(date +"%T") "size " ${SIZE} MiB # start time

for IO in $(seq ${IO_START} ${IO_END})
do 
  export PARENT_DIR=${SLURM_SUBMIT_DIR}/${DIR}/${SLURM_NNODES}_${SLURM_NTASKS_PER_NODE}/${SIZE}MiB/${IOLAYERS[${IO}]}

  ## Case 1; sequential case 
  export UCX_IB_REG_METHODS=direct
  source ${SLURM_SUBMIT_DIR}/slurm_files/sequential.sh
  wait 
  # Case 2; shared memory in HT 
  export UCX_IB_REG_METHODS=direct
  export FLAG="shared"
  source ${SLURM_SUBMIT_DIR}/slurm_files/hyperthread.sh 
  wait 
  # Case 3; message passing in HT 
  export UCX_IB_REG_METHODS=direct
  export FLAG="HT"
  source ${SLURM_SUBMIT_DIR}/slurm_files/hyperthread.sh 
  wait 
  # Case 4; shared memory in consecutive 
  export UCX_IB_REG_METHODS=direct
  export FLAG="shared"
  source ${SLURM_SUBMIT_DIR}/slurm_files/consecutive.sh
  wait 
  # Case 5; message passing in consecutive 
  export UCX_IB_REG_METHODS=direct
  export FLAG="HT"
  source ${SLURM_SUBMIT_DIR}/slurm_files/consecutive.sh
  wait 

done 

echo $(module list) 

echo "Job ended " $(date +"%T") # end time 
