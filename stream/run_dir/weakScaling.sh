# directory 
# time for job  
TIMES=("3:00:00" "03:00:00" "04:00:00" "05:00:00" "05:00:00" "05:00:00") 

echo $DIR 
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
  echo NODES ${NUM_NODES} ARRAY SIZE ${NX} x ${NY}  TIME ${TIME_VAR} IO ${IO_START} to ${IO_END} 

  sbatch --export=ALL,SIZE=${SIZE_LOCAL},NX=${NX},NY=${NY},DIR=${DIR},IO_START=${IO_START},IO_END=${IO_END},FLAG=${FLAG} --qos=standard --nodes=${NUM_NODES} --ntasks-per-node=${PPN} --time=${TIME_VAR} --array=${ARRAY} --wait archer2.slurm 
  
 # wait 

  # sbatch --export=ALL,NX=${NX},NY=${NY},DIR=${DIR},IO_START=${IO_START},IO_END=${IO_END},--qos=standard --nodes=${NUM_NODES} --ntasks-per-node=${PPN} --time=${TIME_VAR} test.slurm 

done 

