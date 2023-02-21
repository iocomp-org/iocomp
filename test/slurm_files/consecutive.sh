export CASE=Consecutive
export RUNDIR=${PARENT_DIR}/${CASE}/$i
echo "**" $CASE 
rm -rf ${RUNDIR}
mkdir -p ${RUNDIR}
lfs setstripe -c -1  ${RUNDIR}
cd ${RUNDIR} 

if (( ${SLURM_NNODES} > 2  )); then 
    srun --cpu-bind=verbose --hint=nomultithread  --distribution=block:block --ntasks=${FULL_CORES} --nodes=${SLURM_NNODES} xthi > test.out
else
    srun --cpu-bind=verbose --hint=nomultithread  --distribution=block:block --ntasks=${FULL_CORES} --nodes=${SLURM_NNODES} --cpu-bind=map_cpu:${bar[@]} xthi > test.out
fi 

module list  2>&1 | tee -a test.out 
echo "JOB ID"  $SLURM_JOBID >> test.out
echo "JOB NAME" ${SLURM_JOB_NAME} >> test.out
