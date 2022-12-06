#!/bin/bash --login
#SBATCH --job-name=iocomp
#SBATCH --nodes=1
#SBATCH --tasks-per-node=128
#SBATCH --cpus-per-task=1
#SBATCH --time=00:10:00
#SBATCH --account=e609
#SBATCH --partition=standard 
#SBATCH --qos=lowpriority

#module load cray-hdf5-parallel
#module load cray-python 
module swap PrgEnv-cray/8.0.0 PrgEnv-gnu
module load cmake 
module use /work/y07/shared/archer2-lmod/dev
module load xthi 
module swap craype-network-ofi craype-network-ucx 
module swap cray-mpich cray-mpich-ucx 

# Setup environment
export PPN=${SLURM_NTASKS_PER_NODE}
export OMP_NUM_THREADS=1
#export RUNDIR=/work/e609/e609/shr203/iocomp/test/run_dir/${SLURM_NNODES}_${SLURM_NTASKS_PER_NODE}
export PARENT_DIR=/work/e609/e609/shr203/iocomp/test/test_dir/28Nov/${SLURM_NNODES}_${SLURM_NTASKS_PER_NODE}
export TEST=/work/e609/e609/shr203/iocomp/test/test
export IOCOMP_DIR=/work/e609/e609/shr203/iocomp
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${IOCOMP_DIR}/lib

export FULL_CORES=${SLURM_NTASKS_PER_NODE}
export HALF_CORES=$((${FULL_CORES}/2)) 

for i in {1..1}; do 
  # Serial case 
  export RUNDIR=${PARENT_DIR}/Serial/$i
  rm -rf ${RUNDIR}
  mkdir -p ${RUNDIR}
  lfs setstripe -c -1  ${RUNDIR}
  cd ${RUNDIR} 
  end=$((${HALF_CORES}-1))
  vals=($(seq 0 1 $(eval echo ${end})))
  bar=$(IFS=, ; echo "${vals[*]}")
  srun --cpu-bind=verbose --hint=nomultithread  --distribution=block:block --ntasks=${HALF_CORES} --cpu-bind=map_cpu:${bar[@]} --nodes=1  ${TEST} > test.out

 # Overcommit case 
  export RUNDIR=${PARENT_DIR}/Overcommit/$i
  rm -rf ${RUNDIR}
  mkdir -p ${RUNDIR}
  lfs setstripe -c -1  ${RUNDIR}
  cd ${RUNDIR} 
  end=$((${HALF_CORES}-1))
  vals=($(seq 0 1 $(eval echo ${end})))
  bar=$(IFS=, ; echo "${vals[*]}")
  srun  --cpu-bind=verbose --hint=nomultithread --distribution=block:block --ntasks=${FULL_CORES} --cpu-bind=map_cpu:${bar[@]} --nodes=1 --overcommit ${TEST} --HT > test.out

  # Hyperthreaded 
  export RUNDIR=${PARENT_DIR}/Hyperthread/$i
  rm -rf ${RUNDIR}
  mkdir -p ${RUNDIR}
  lfs setstripe -c -1  ${RUNDIR}
  cd ${RUNDIR} 
  # seq 1
  end=$((${HALF_CORES}-1))
  vals=($(seq 0 1 $(eval echo ${end})))
  # seq 2 
  end=$((${HALF_CORES}+128-1))
  start=128
  vals_HT=($(seq $(eval echo ${start}) 1 $(eval echo ${end})))
  updated=("${vals[@]}" "${vals_HT[@]}")
  bar=$(IFS=, ; echo "${updated[*]}")
  srun  --cpu-bind=verbose --hint=multithread --distribution=block:block --ntasks=${FULL_CORES} --nodes=1 --cpu-bind=map_cpu:${bar[@]} ${TEST} --HT > test.out

  # Consecutive cores 
  export RUNDIR=${PARENT_DIR}/Consecutive/$i
  rm -rf ${RUNDIR}
  mkdir -p ${RUNDIR}
  lfs setstripe -c -1  ${RUNDIR}
  cd ${RUNDIR} 
  echo "** Consecutive cores"
  end=$((${FULL_CORES}-1))
  vals=($(seq 0 1 $(eval echo ${end})))
  bar=$(IFS=, ; echo "${vals[*]}")
  srun --cpu-bind=verbose --hint=nomultithread  --distribution=block:block --ntasks=${FULL_CORES} --cpu-bind=map_cpu:${bar[@]} --nodes=1 ${TEST} --HT > test.out
done 

module list  2>&1 | tee -a test.out 

echo "JOB ID"  $SLURM_JOBID >> test.out
echo "JOB NAME" ${SLURM_JOB_NAME} >> test.out


