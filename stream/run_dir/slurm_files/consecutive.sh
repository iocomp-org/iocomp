export CASE=Consecutive/${FLAG} 

# setup of directories and copying of config files and make outputs.
source ${SLURM_SUBMIT_DIR}/slurm_files/setup.sh 

END_CORES=${SLURM_NTASKS_PER_NODE}

# Generate sequence from 0 to PPN 
end=$((${END_CORES}-1))
vals=($(seq 0 1 $(eval echo ${end})))
bar=$(IFS=, ; echo "${vals[*]}")


if (( ${MAP} == 1  )); then 
  TOTAL_RANKS=$(( ${SLURM_NNODES} * ${END_CORES} ))
  map --mpi=slurm -n ${TOTAL_RANKS} --mpiargs="--hint=nomultithread  --distribution=block:block --cpu-bind=map_cpu:${bar[@]}" --profile  ${EXE} --${FLAG} --nx ${NX} --ny ${NY}  --io ${IO} > test.out 
else 
  srun  --hint=nomultithread  --distribution=block:block --cpu-bind=map_cpu:${bar[@]} ${EXE} --${FLAG} --nx ${NX} --ny ${NY}  --io ${IO}  > test.out 
  wait 
  # for testing purposes, global number of nodes/processes are halved from  to match the ranks available to IO server
  NUM_NODES=${SLURM_NNODES} 
  if (( ${NUM_NODES} > 1  )); then 
    NODES_TEST=$((${NUM_NODES}/2))
    PPN_TEST=${SLURM_NTASKS_PER_NODE}
  else
    NODES_TEST=${NUM_NODES}
    PPN_TEST=$((${SLURM_NTASKS_PER_NODE}/2)) 
  fi 
  TASKS_TEST=$(( ${NODES_TEST} * ${PPN_TEST} )) 
  echo TESTING with ${NODES_TEST} nodes and ${TASKS_TEST} tasks ${PPN_TEST} tasks per node.
  srun  --nodes=${NODES_TEST} --ntasks-per-node=${PPN_TEST} --ntasks=${TASKS_TEST} ${TEST_EXE} --nx ${NX} --ny ${NY}  --io ${IO} >> test.out
  wait 
fi 

echo "JOB ID"  $SLURM_JOBID >> test.out
echo "JOB NAME" ${SLURM_JOB_NAME} >> test.out
