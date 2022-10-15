#!/bin/bash --login
#SBATCH --job-name=iocomp
#SBATCH --nodes=1
#SBATCH --tasks-per-node=128
#SBATCH --cpus-per-task=1
#SBATCH --time=0:10:00
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
export RUNDIR=/work/e609/e609/shr203/iocomp/test/run_dir/${SLURM_NNODES}_${SLURM_NTASKS_PER_NODE}
export TEST=/work/e609/e609/shr203/iocomp/test/test
export IOCOMP_DIR=/work/e609/e609/shr203/iocomp
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${IOCOMP_DIR}/lib

rm -rf ${RUNDIR}
mkdir -p ${RUNDIR}
lfs setstripe -c -1  ${RUNDIR}

cd ${RUNDIR} 
#srun --hint=nomultithread --oversubscribe --ntasks=128 --distribution=block:block xthi 
#srun --hint=nomultithread --oversubscribe --ntasks=128 --distribution=block:block:cyclic  xthi 
#srun  --oversubscribe --ntasks=256 --distribution=block:block:cyclic  xthi
#srun  --oversubscribe --ntasks=256 --distribution=block:block xthi 

srun --hint=nomultithread --distribution=block:block ${TEST} > test.out
module list  2>&1 | tee -a test.out 

echo "JOB ID"  $SLURM_JOBID >> test.out
echo "JOB NAME" ${SLURM_JOB_NAME} >> test.out


