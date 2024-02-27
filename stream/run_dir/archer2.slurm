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

# set -euo pipefail # error handling for bash 

# different module loads. Swap cray env to gnu env 
module swap PrgEnv-cray PrgEnv-gnu
module use /work/y07/shared/archer2-lmod/dev
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

# avg jobs directories
ITER=${SLURM_ARRAY_TASK_ID}

# flag for archer2 runs 
export FI_OFI_RXM_SAR_LIMIT=64K

## for darshan runs 

# Setup environment
export PPN=${SLURM_NTASKS_PER_NODE}
export N_NODES=${SLURM_NNODES}
export OMP_NUM_THREADS=1

if (( ${MAP} == 1 )); 
then 
  module load arm/forge 
fi 

IOLAYERS=("MPIIO" "HDF5" "ADIOS2_HDF5" "ADIOS2_BP4" "ADIOS2_BP5") # assign IO layer array 
  CASES=("Sequential" "Hyperthread/split" "Hyperthread/shared" "Consecutive/split" "Consecutive/shared")
  SIZE=$(( ${NX}*${NY}*8 / 2**20 )) # local size in MiB

  echo "Job started " $(date +"%T") "size " ${SIZE} MiB # start time

# if variables not set then set them to be MPIIO and all the cases. 
if [ -z ${IO_START} ] && [ -z ${IO_END} ]
then 
  export IO_START=0
  export IO_END=0
fi 

if [ -z ${CASE_START} ] && [ -z ${CASE_END} ]
then 
  export CASE_START=0
  export CASE_END=4
fi 

for IO in $(seq ${IO_START} ${IO_END})
do 
  for MAPPING_CASE in $( seq ${CASE_START} ${CASE_END} ) 
  do 

    export WORKING_DIR=${DIR}/${SLURM_NNODES}_${SLURM_NTASKS_PER_NODE}/${SIZE}MiB/${IOLAYERS[${IO}]}/${CASES[${MAPPING_CASE}]}/${ITER}
    echo ${WORKING_DIR}  
    # create directories, max stripings etc. 
    source ${SLURM_SUBMIT_DIR}/bash_scripts/setup.sh

    # select appropriate mapping case 
    echo ${CASES[${MAPPING_CASE}]} ' selected' 
    if (( ${MAPPING_CASE} == 0 )); 
    then 
      export HINT="nomultithread"
      export FLAG="" 
      source ${SLURM_SUBMIT_DIR}/bash_scripts/sequential.sh 
    elif (( ${MAPPING_CASE} == 1 )); 
    then
      export FLAG="HT"
      export HINT="multithread"
      source ${SLURM_SUBMIT_DIR}/bash_scripts/hyperthread.sh 
    elif (( ${MAPPING_CASE} == 2 )); 
    then 
      export FLAG="shared"
      export HINT="multithread"
      source ${SLURM_SUBMIT_DIR}/bash_scripts/hyperthread_shared.sh 
    elif (( ${MAPPING_CASE} == 3 )); 
    then 
      export FLAG="HT"
      export HINT="nomultithread"
      source ${SLURM_SUBMIT_DIR}/bash_scripts/consecutive_nodesplit.sh
    elif (( ${MAPPING_CASE} == 4 )); 
    then 
      export FLAG="shared"
      export HINT="nomultithread"
      source ${SLURM_SUBMIT_DIR}/bash_scripts/consecutive.sh
    else
      echo 'Invalid mapping case number' 
    fi 
    if (( ${MAPPING_CASE} != 0 )); # don't wait for sequential 
    then 
      wait 
    fi 

    echo "nodes" $NUM_NODES "tasks" $NUM_TASKS

    export FI_OFI_RXM_SAR_LIMIT=64K 
    export MPICH_MPIIO_HINTS=":cray_cb_write_lock_mode=2,:cray_cb_nodes_multiplier=4"

    if (( ${DARSHAN} == 1 )); 
    then 
      export DXT_ENABLE_IO_TRACE=1 # darshan DXT trace 
      module load darshan
    fi 

    NUM_TASKS_PER_NODE=$(( ${NUM_TASKS}/${NUM_NODES} ))  # to avoid srun complaining for hyperthread cases 
    srun  --hint=${HINT} --distribution=block:block  --nodes=${NUM_NODES} --ntasks=${NUM_TASKS} \
      --ntasks-per-node=${NUM_TASKS_PER_NODE}\
      --cpu-bind=map_cpu:${seq[@]} ${EXE} --${FLAG} --nx ${NX} --ny ${NY}  --io ${IO} > test.out 

    # wait and delete data 
    wait 
  
    # get darshan file from the folder and copy to the right one.
    if (( ${DARSHAN} == 1 )); 
    then 
      module remove darshan
      day=$( date +%-d )
      month=$( date +%-m )
      mv /mnt/lustre/a2fs-nvme/system/darshan/2024/${month}/${day}/shr203*.darshan .  
    fi 

    find ./ -name "*.dat" -exec rm -rf {} \; 
    find ./ -name "*.h5" -exec rm -rf {} \; 
    find ./ -name "*.bp4" -exec rm -rf {} \; -prune
    # copy the latest darshan file to current directory

  done  # mapping case loop
done # IO loop 

echo $(module list) 
echo "Job ended " $(date +"%T") # end time 
