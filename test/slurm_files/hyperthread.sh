export CASE=Hyperthread
export RUNDIR=${PARENT_DIR}/${CASE}/$i
echo "**" $CASE 
rm -rf ${RUNDIR}
mkdir -p ${RUNDIR}
lfs setstripe -c -1  ${RUNDIR}
cd ${RUNDIR} 

# seq 1
end=$((${HALF_CORES}-1))
vals=($(seq 0 1 $(eval echo ${end})))

# seq 2 
end=$((${HALF_CORES}+128-1))
start=128
vals_HT=($(seq $(eval echo ${start}) 1 $(eval echo ${end})))
updated=("${vals[@]}" "${vals_HT[@]}")
bar=$(IFS=, ; echo "${updated[*]}")

if (( ${SLURM_NNODES} > 2  )); then 
    srun  --cpu-bind=verbose --hint=multithread --distribution=block:block --ntasks=${FULL_CORES} --nodes=${HALF_NODES} --cpu-bind=map_cpu:${bar[@]} xthi > test.out    
else
  srun  --cpu-bind=verbose --hint=multithread --distribution=block:block --ntasks=${FULL_CORES} --nodes=${SLURM_NNODES} --cpu-bind=map_cpu:${bar[@]} xthi > test.out
fi 

module list  2>&1 | tee -a test.out 
echo "JOB ID"  $SLURM_JOBID >> test.out
echo "JOB NAME" ${SLURM_JOB_NAME} >> test.out
