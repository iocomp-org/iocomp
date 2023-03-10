export CASE=Consecutive
export RUNDIR=${PARENT_DIR}/${CASE}/$i
echo "**" $CASE 
rm -rf ${RUNDIR}
mkdir -p ${RUNDIR}
lfs setstripe -c -1  ${RUNDIR}
cd ${RUNDIR} 
cp ${CONFIG} . 

end=$((${FULL_CORES}-1))
vals=($(seq 0 1 $(eval echo ${end})))
bar=$(IFS=, ; echo "${vals[*]}")

srun  --hint=nomultithread  --distribution=block:block  --nodes=${SLURM_NNODES} --cpu-bind=map_cpu:${bar[@]} ${EXE} --HT 

module list  2>&1 | tee -a test.out 
echo "JOB ID"  $SLURM_JOBID >> test.out
echo "JOB NAME" ${SLURM_JOB_NAME} >> test.out
