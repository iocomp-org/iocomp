export CASE=Consecutive

# setup of directories and copying of config files and make outputs.
source ${SLURM_SUBMIT_DIR}/slurm_files/setup.sh 

end=$((${FULL_CORES}-1))
vals=($(seq 0 1 $(eval echo ${end})))
bar=$(IFS=, ; echo "${vals[*]}")

#if (( ${MAP} == 1  )); then 
#  TOTAL_RANKS=$(( ${SLURM_NNODES} * ${FULL_CORES} ))
#  map --mpi=slurm -n ${TOTAL_RANKS} --mpiargs="--hint=nomultithread  --distribution=block:block" --profile  ${EXE} --HT --nx ${NX} --ny ${NY}  --io ${IO}
#else 
#srun  --hint=nomultithread  --distribution=block:block  --nodes=${SLURM_NNODES} --cpu-bind=map_cpu:${bar[@]} ${EXE} --HT --nx ${NX} --ny ${NY}  --io ${IO} > test.out 
#fi 

srun ${EXE} --nx ${NX} --ny ${NY} --io ${IO} --${FLAG} > test.out 

wait 

# halve the global size if flag is activated as only half ranks will be writing. 
if [[ -n ${FLAG} ]];
then 
  NX_TEST=$(( ${NX}/2 )) 
else
  NX_TEST=${NX}
fi 

srun ${TEST_EXE} --nx ${NX_TEST} --ny ${NY} --io ${IO}  >> test.out 

echo "JOB ID"  $SLURM_JOBID >> test.out
echo "JOB NAME" ${SLURM_JOB_NAME} >> test.out
