# directory 
# time for job  
TIMES=("5:00:00" "6:00:00" "7:00:00" "8:00:00" "10:00:00" "14:00:00" "20:00:00" "24:00:00" "24:00:00") 
echo ${DIR} 
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
  NUM_NODES=$((2**${i}))
  echo NODES ${NUM_NODES} ARRAY SIZE ${NX} x ${NY}  TIME ${TIME_VAR} IO ${IO_START} to ${IO_END} MAP ${MAP} CASE ${CASE_START} to ${CASE_END} 

  sbatch --export=ALL,SIZE=${SIZE_LOCAL},NX=${NX},NY=${NY},DIR=${DIR},IO_START=${IO_START},IO_END=${IO_END},FLAG=${FLAG},MAP=${MAP},CASE_START=${CASE_START},CASE_END=${CASE_END},DARSHAN=${DARSHAN} --qos=standard --nodes=${NUM_NODES} --ntasks-per-node=${PPN} --time=${TIME_VAR} --array=${ARRAY} ${WAIT} archer2.slurm 
  
  wait 
done 

