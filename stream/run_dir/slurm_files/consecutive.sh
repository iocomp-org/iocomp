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
  srun  --hint=nomultithread  --distribution=block:block --cpu-bind=map_cpu:${bar[@]} ${EXE} --${FLAG} --nx ${NX} --ny ${NY}  --io ${IO} > test.out 
  wait 
  # for testing purposes, global size is halved to match the actual number of writers.
  NX_TEST=$((${NX}/2))
  srun ${TEST_EXE} --nx ${NX_TEST} --ny ${NY}  --io ${IO} >> test.out 
  wait 
fi 

echo "JOB ID"  $SLURM_JOBID >> test.out
echo "JOB NAME" ${SLURM_JOB_NAME} >> test.out
