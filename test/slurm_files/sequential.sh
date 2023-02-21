export CASE=Sequential
export RUNDIR=${PARENT_DIR}/${CASE}/$i
echo "**" $CASE 
rm -rf ${RUNDIR}
mkdir -p ${RUNDIR}
lfs setstripe -c -1  ${RUNDIR}
cd ${RUNDIR} 
end=$((${HALF_CORES}-1))
vals=($(seq 0 1 $(eval echo ${end})))
bar=$(IFS=, ; echo "${vals[*]}")

if (( ${SLURM_NNODES} > 2  )); then 
  srun --cpu-bind=verbose --hint=nomultithread  --distribution=block:block --ntasks=${HALF_CORES} --nodes=${HALF_NODES} xthi > test.out
else
  srun --cpu-bind=verbose --hint=nomultithread  --distribution=block:block --ntasks=${HALF_CORES} --nodes=${SLURM_NNODES}  --cpu-bind=map_cpu:${bar[@]}  xthi > test.out 
fi 
module list  2>&1 | tee -a test.out 

echo "JOB ID"  $SLURM_JOBID >> test.out
echo "JOB NAME" ${SLURM_JOB_NAME} >> test.out
