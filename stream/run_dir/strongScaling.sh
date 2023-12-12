# strong scaling 

TIMES=("5:00:00" "04:00:00" "03:00:00" "03:00:00" "03:00:00" "03:00:00" "03:00:00") 
MAX_NODES=1

# so that global size is always constant, irrespective of what is NODE start 
NX_LOCAL=$((${NX}*2/(2**${NODE_START}))) # multiplying by 2 cos strong scaling halves the array for remainder 0.  
NY_LOCAL=${NY}
iter=${NODE_START}  
# iterate through number of nodes 
for i in $(seq ${NODE_START} ${NODE_END}) # 1 till 16 nodes 
do 
  # check if TIME variable is set from runscript. If not then set it from the array. 
  if [[ -n $TIME ]];
  then 
    TIME_VAR=${TIME} 
  else
    TIME_VAR=${TIMES[${i}]} 
  fi 

  # get local values from the global specified for the number of nodes given. 
  # balance arrays so that no array is less than 16
  NUM_NODES=$((2**${i}))

  if [[ $(( $iter % 2)) == 0 ]] ; 
  then 
    NX_LOCAL=$((${NX_LOCAL}/2 )) 
  fi 
  if [[ $(( $iter % 2)) == 1 ]] ; 
  then 
    NY_LOCAL=$((${NY_LOCAL}/2 )) 
  fi 
  
  ((iter++))

  FILESIZE_GLOBAL=$((${NX} * ${NY} * 8 * 128/ 2**20)) 
  FILESIZE_LOCAL=$((${NX_LOCAL} * ${NY_LOCAL} * 8/ 2**20)) 
  FILESIZE_GLOBAL_CALCULATED=$((${FILESIZE_LOCAL} * ${NUM_NODES} * ${PPN})) # 1/2 no. of compute processes

  echo NODES ${NUM_NODES} ARRAY SIZE ${NX_LOCAL} x ${NY_LOCAL} Local size ${FILESIZE_LOCAL}MiB Global size correct ${FILESIZE_GLOBAL}MiB and calculated ${FILESIZE_GLOBAL_CALCULATED}MiB  TIME ${TIME_VAR} IO ${IO_START} to ${IO_END} JOBS ${ARRAY} MAP ${MAP} CASE ${CASE_START} to ${CASE_END}
  
  # sbatch --export=ALL,NX=${NX_LOCAL},NY=${NY_LOCAL},DIR=${DIR},IO_START=${IO_START},IO_END=${IO_END},FLAG=${FLAG},MAP=${MAP},CASE_START=${CASE_START},CASE_END=${CASE_END} --qos=standard --nodes=${NUM_NODES} --ntasks-per-node=${PPN} --time=${TIME_VAR} --array=${ARRAY} archer2.slurm 
done 

