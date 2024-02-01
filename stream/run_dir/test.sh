set -o errexit   # abort on nonzero exitstatus
# set -o nounset   # abort on unbound variable
set -o pipefail  # don't hide errors within pipes

export SLURM_NNODES=1
export SLURM_NTASKS_PER_NODE=16
export N_NODES=${SLURM_NNODES}
export PPN=${SLURM_NTASKS_PER_NODE}
# source functions 

seq=""

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
    
    if (( ${MAPPING_CASE} == 0 )); then 
      echo 'Sequential selected' 
      source ./bash_scripts/sequential.sh 
    fi 

    if (( ${MAPPING_CASE} == 1 )); then
      echo 'Hyperthread 1 selected' 
      source ./bash_scripts/hyperthread.sh 
    fi 

    if (( ${MAPPING_CASE} == 2 )); then 
      echo 'Hyperthread 2 selected' 
      source ./bash_scripts/hyperthread_shared.sh 
    fi  

    if (( ${MAPPING_CASE} == 3 )); then 
      echo 'Consecutive 1 selected' 
      source ./bash_scripts/consecutive.sh
    fi 

    if (( ${MAPPING_CASE} == 4 )); then 
      echo 'Consecutive 2 selected' 
      source ./bash_scripts/consecutive.sh
    fi 
    echo $seq 
    
  done  # mapping case loop
done # IO loop 
