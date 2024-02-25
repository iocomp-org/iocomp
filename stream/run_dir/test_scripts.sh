if [ -e $1 ]; then 
  export N_NODES=$1
else
  export N_NODES=2
fi 

if [ -e $2 ]; then 
  export PPN=$2
else
  export PPN=16
fi 
export SLURM_SUBMIT_DIR=$(pwd)

CASES=("Sequential" "Hyperthread/split" "Hyperthread/shared" "Consecutive/split" "Consecutive/shared")
for MAPPING_CASE in $(seq 0 4)
do 
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
      source ${SLURM_SUBMIT_DIR}/bash_scripts/consecutive.sh
    elif (( ${MAPPING_CASE} == 4 )); 
    then 
      export FLAG="shared"
      export HINT="nomultithread"
      source ${SLURM_SUBMIT_DIR}/bash_scripts/consecutive.sh
    else
      echo 'Invalid mapping case number' 
    fi 
    echo $seq

done 

