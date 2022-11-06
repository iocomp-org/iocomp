#!/bin/bash --login
#SBATCH --job-name=iocomp
#SBATCH --nodes=1
#SBATCH --tasks-per-node=2
#SBATCH --cpus-per-task=1
#SBATCH --time=2:00:00
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
export PARENT_DIR=/work/e609/e609/shr203/iocomp/test/run_dir/5Nov
export TEST=/work/e609/e609/shr203/iocomp/test/test
export IOCOMP_DIR=/work/e609/e609/shr203/iocomp
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${IOCOMP_DIR}/lib

for i in {1..10}; do 
  # Serial case 
  export RUNDIR=${PARENT_DIR}/Serial/$i
  rm -rf ${RUNDIR}
  mkdir -p ${RUNDIR}
  lfs setstripe -c -1  ${RUNDIR}
  cd ${RUNDIR} 
  srun --hint=nomultithread --distribution=block:block --ntasks=1 ${TEST} > test.out

  # Overcommit case 
  export RUNDIR=${PARENT_DIR}/Overcommit/$i
  rm -rf ${RUNDIR}
  mkdir -p ${RUNDIR}
  lfs setstripe -c -1  ${RUNDIR}
  cd ${RUNDIR} 
  srun --overcommit --ntasks-per-node=2 ${TEST} > test.out

  # Hyperthreaded 
  export RUNDIR=${PARENT_DIR}/Hyperthread/$i
  rm -rf ${RUNDIR}
  mkdir -p ${RUNDIR}
  lfs setstripe -c -1  ${RUNDIR}
  cd ${RUNDIR} 
  srun --hint=multithread --distribution=block:block ${TEST} > test.out

  # Consecutive cores 
  export RUNDIR=${PARENT_DIR}/Consecutive/$i
  rm -rf ${RUNDIR}
  mkdir -p ${RUNDIR}
  lfs setstripe -c -1  ${RUNDIR}
  cd ${RUNDIR} 
  srun --ntasks=2 --cpus-per-task=1 --distribution=block:block --hint=nomultithread ${TEST} > test.out
done 

module list  2>&1 | tee -a test.out 

echo "JOB ID"  $SLURM_JOBID >> test.out
echo "JOB NAME" ${SLURM_JOB_NAME} >> test.out


