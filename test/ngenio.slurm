#!/bin/bash --login
#SBATCH -J test.job
#SBATCH --nodes=4
#SBATCH --tasks-per-node=48
#SBATCH --time=03:00:00
#SBATCH --nvram-options=1LM:1000

export OMP_NUM_THREADS=1
export I_MPI_PIN_PROCESSOR_LIST=0-47
export PSM2_DEVICES="self,hfi,shm"
export PSM2_MULTIRAIL=1
export PSM2_MULTIRAIL_MAP=0:1,1:1
export TEST=/home/nx01/nx01/s203/iocomp/iocomp
export RUN_DIR=/home/nx01/nx01/s203/iocomp/run_dir

module load gnu8 
#module load openmpi/4.0.3-GNU8
module load mvapich2
rm -rf ${RUN_DIR} 
cd ${RUN_DIR}

mpirun -n 4 ${TEST}

