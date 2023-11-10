export CASE=Hyperthread/${FLAG}

# setup of directories and copying of config files and make outputs.
source ${SLURM_SUBMIT_DIR}/slurm_files/setup.sh 

# if more than 1 node, then HT uses half the number of nodes. 
# If less than 1 node, then HT uses half the number of cores.
if (( ${SLURM_NNODES} > 1  )); then 
  NUM_NODES=$((${SLURM_NNODES}/2))  
  END_CORES=${SLURM_NTASKS_PER_NODE}
else
  NUM_NODES=${SLURM_NNODES} 
  END_CORES=$((${SLURM_NTASKS_PER_NODE}/2))
fi 

# Generate sequence such that the normal cores and HT cores(normal cores + 128) are stacked and seperated with a comma.   
# seq 1 
end=$((${END_CORES}-1))
vals=($(seq 0 1 $(eval echo ${end})))

# seq 2 
end=$((${END_CORES}+128-1))
start=128
vals_HT=($(seq $(eval echo ${start}) 1 $(eval echo ${end})))
updated=("${vals[@]}" "${vals_HT[@]}")
bar=$(IFS=, ; echo "${updated[*]}")

if (( ${MAP} == 1  )); then
  TOTAL_RANKS=$(( ${NUM_NODES} * ${END_CORES} )) 
  map --mpi=slurm -n ${TOTAL_RANKS} --mpiargs="--hint=multithread --distribution=block:block  --nodes=${NUM_NODES} --cpu-bind=map_cpu:${bar[@]}" --profile ${EXE} --${FLAG} --nx ${NX} --ny ${NY}  --io ${IO} > test.out
else
  srun  --hint=multithread --distribution=block:block  --nodes=${NUM_NODES} --cpu-bind=map_cpu:${bar[@]} ${EXE} --${FLAG} --nx ${NX} --ny ${NY}  --io ${IO} > test.out 
  wait  
  # for testing purposes, global number of nodes/processes are halved from  to match the ranks available to IO server
  #if (( ${NUM_NODES} > 1  )); then 
  #  NODES_TEST=$((${NUM_NODES}/2))
  #  PPN_TEST=${SLURM_NTASKS_PER_NODE}
  #else
  #  NODES_TEST=${NUM_NODES}
  #  PPN_TEST=$((${SLURM_NTASKS_PER_NODE}/2)) 
  #fi 
  NODES_TEST=${NUM_NODES}
  PPN_TEST=${END_CORES} 
  #TASKS_TEST=$(( ${NODES_TEST} * ${PPN_TEST} )) 
  #echo TESTING with ${NODES_TEST} nodes and ${TASKS_TEST} tasks ${PPN_TEST} tasks per node.
  #srun  --nodes=${NODES_TEST} --ntasks-per-node=${PPN_TEST} --ntasks=${TASKS_TEST} ${TEST_EXE} --nx ${NX} --ny ${NY}  --io ${IO} >> test.out
  #wait 
fi 

echo "JOB ID"  $SLURM_JOBID >> test.out
echo "JOB NAME" ${SLURM_JOB_NAME} >> test.out
