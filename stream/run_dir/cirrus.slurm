#!/bin/bash --login
#SBATCH --job-name=stream
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=36
#SBATCH --cpus-per-task=1
#SBATCH --time=00:10:00
#SBATCH --account=d403-sb
#SBATCH --partition=standard 
#SBATCH --qos=lowpriority
#SBATCH --exclusive 

module load gcc/8.2.0 
module load mpt 
module load hdf5parallel/1.10.6-gcc8-mpt225

# export HDF5_DIR=/mnt/lustre/indy2lfs/sw/hdf5parallel/1.10.6-gcc8-mpt225
# export LD_LIBRARY_PATH=$HDF5_DIR/lib:$LD_LIBRARY_PATH
export IOCOMP_DIR=/work/d403/d403/shr2032/opt/gcc/iocomp/1.1.3
export LD_LIBRARY_PATH=${IOCOMP_DIR}/lib:${LD_LIBRARY_PATH}
export EXE=${IOCOMP_DIR}/stream/test

export OMP_NUM_THREADS=1
export SLURM_NTASKS_PER_NODE=72

export FULL_CORES=72
export NODESIZE=36
export HALF_CORES=$((${FULL_CORES}/2))
export HALF_NODES=$((${SLURM_NNODES}/2))
export PARENT_DIR=${SLURM_SUBMIT_DIR}/TEST
export MAP=0 

# if parameters are not set, then default parameters are given 
if [ -z ${IO} ] 
then 
  export IO=0  # MPIIO 
fi 

if [ -z ${nx} ] 
then 
  export nx=1024
fi 

if [ -z ${ny} ] 
then 
  export ny=1024
fi 

source ${SLURM_SUBMIT_DIR}/slurm_files/sequential.sh 
