# strong scaling 

TIMES=("5:00:00" "04:00:00" "03:00:00" "03:00:00" "03:00:00" "03:00:00" "03:00:00") 
MAX_NODES=1
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

  # get local values from the global specified for the number of nodes given 
  NUM_NODES=$((2**${i}))
  NX_LOCAL=$((${NX}*${MAX_NODES}/${NUM_NODES})) 
  FILESIZE_LOCAL=$((${NX_LOCAL} * ${NY} * 8/ 2**20)) 
  FILESIZE_GLOBAL=$(( ${NX_LOCAL} * ${NY} * 8 * ${NUM_NODES} * 128 / (2*2**20) ))  # global size is halfed due to IO servers

  echo NODES ${NUM_NODES} ARRAY SIZE ${NX_LOCAL} x ${NY} Local size ${FILESIZE_LOCAL}MiB Global size ${FILESIZE_GLOBAL}MiB  TIME ${TIME_VAR} IO ${IO_START} to ${IO_END} JOBS ${ARRAY}
  
  sbatch --export=ALL,NX=${NX_LOCAL},NY=${NY},DIR=${DIR},IO_START=${IO_START},IO_END=${IO_END},FLAG=${FLAG} --qos=standard --nodes=${NUM_NODES} --ntasks-per-node=${PPN} --time=${TIME_VAR} --array=${ARRAY}  archer2.slurm 
done 

