export CASE=Sequential

# setup of directories and copying of config files and make outputs.
source ${SLURM_SUBMIT_DIR}/slurm_files/setup.sh 

if (( ${SLURM_NNODES} > 1  )); then 
  NUM_NODES=$((${SLURM_NNODES}/2))  
  END_CORES=${SLURM_NTASKS_PER_NODE}
  # half the number of total tasks divided between the allocated nodes.
  NUM_TASKS=$((${SLURM_NNODES}*${SLURM_NTASKS_PER_NODE}/2))  
else 
  NUM_NODES=${SLURM_NNODES} 
  END_CORES=$((${SLURM_NTASKS_PER_NODE}/2))
  NUM_TASKS=$((${SLURM_NTASKS_PER_NODE}/2))  
fi

# sequencing for cpu bind 
end=$((${END_CORES}-1))
vals=($(seq 0 1 $(eval echo ${end})))
bar=$(IFS=, ; echo "${vals[*]}")

if (( ${MAP} == 1  )); then 
  TOTAL_RANKS=$((${NUM_NODES}*${END_CORES}))
  map --mpi=slurm -n ${TOTAL_RANKS} --mpiargs="--hint=nomultithread  --distribution=block:block --nodes=${NUM_NODES} --ntasks=${HALF_CORES} --cpu-bind=map_cpu:${bar[@]}" --profile  ${EXE} --nx ${NX} --ny ${NY} --io ${IO}
else 
  srun  --hint=nomultithread  --distribution=block:block --nodes=${NUM_NODES} --ntasks=${NUM_TASKS} --cpu-bind=map_cpu:${bar[@]} ${EXE} --nx ${NX} --ny ${NY} --io ${IO} > test.out
  wait 
  srun  --hint=nomultithread  --distribution=block:block --nodes=${NUM_NODES} --ntasks=${NUM_TASKS} --cpu-bind=map_cpu:${bar[@]} ${TEST_EXE} --nx ${NX} --ny ${NY} --io ${IO}  >> test.out 
fi 

echo "JOB ID"  $SLURM_JOBID >> test.out
echo "JOB NAME" ${SLURM_JOB_NAME} >> test.out
