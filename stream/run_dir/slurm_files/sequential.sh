export CASE=Sequential

# setup of directories and copying of config files and make outputs.
source ${SLURM_SUBMIT_DIR}/slurm_files/setup.sh 

if (( ${SLURM_NNODES} > 1  )); then 
  NUM_NODES=${HALF_NODES} 
  END_CORES=${FULL_CORES}

  # sequencing for cpu bind 
  end=$((${END_CORES}-1))
  vals=($(seq 0 1 $(eval echo ${end})))
  bar=$(IFS=, ; echo "${vals[*]}")

  HALF_TASKS=$((${SLURM_NNODES}*${SLURM_NTASKS_PER_NODE}/2)) # half the number of total tasks divided between the allocated nodes 
 
  if (( ${MAP} == 1  )); then 
    TOTAL_RANKS=$((${NUM_NODES}*${END_CORES}))
    map --mpi=slurm  -n ${TOTAL_RANKS} --mpiargs="--hint=nomultithread  --distribution=block:block --nodes=${NUM_NODES} --ntasks=${HALF_TASKS}  --cpu-bind=map_cpu:${bar[@]}" ${EXE} --nx ${NX} --ny ${NY} --io ${IO} 
  else 
   srun  --hint=nomultithread  --distribution=block:block --nodes=${NUM_NODES} --ntasks=${HALF_TASKS}  --cpu-bind=map_cpu:${bar[@]} ${EXE} --nx ${NX} --ny ${NY} --io ${IO} > test.out  
 fi 

else
  NUM_NODES=${SLURM_NNODES} 
  END_CORES=${HALF_CORES}
  
  # sequencing for cpu bind 
  end=$((${END_CORES}-1))
  vals=($(seq 0 1 $(eval echo ${end})))
  bar=$(IFS=, ; echo "${vals[*]}")
  
  TOTAL_RANKS=$((${NUM_NODES}*${END_CORES}))
  if (( ${MAP} == 1  )); then 
    map --mpi=slurm -n ${TOTAL_RANKS} --mpiargs="--hint=nomultithread  --distribution=block:block --nodes=${NUM_NODES} --ntasks=${HALF_CORES} --cpu-bind=map_cpu:${bar[@]}" --profile  ${EXE} --nx ${NX} --ny ${NY} --io ${IO}
  else 
    srun  --hint=nomultithread  --distribution=block:block --nodes=${NUM_NODES} --ntasks=${HALF_CORES} --cpu-bind=map_cpu:${bar[@]} ${EXE} --nx ${NX} --ny ${NY} --io ${IO} > test.out
  fi 
fi 

echo "JOB ID"  $SLURM_JOBID >> test.out
echo "JOB NAME" ${SLURM_JOB_NAME} >> test.out
