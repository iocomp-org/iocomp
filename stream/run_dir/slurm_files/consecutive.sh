export CASE=Consecutive
export RUNDIR=${PARENT_DIR}/${CASE}/$i
echo "**" $CASE 
echo $RUNDIR
rm -rf ${RUNDIR}
mkdir -p ${RUNDIR}
lfs setstripe -c -1  ${RUNDIR}
cd ${RUNDIR} 
cp ${CONFIG} . 

end=$((${FULL_CORES}-1))
vals=($(seq 0 1 $(eval echo ${end})))
bar=$(IFS=, ; echo "${vals[*]}")

srun  --hint=nomultithread  --distribution=block:block  --nodes=${SLURM_NNODES} --cpu-bind=map_cpu:${bar[@]} ${EXE} --HT --size ${SIZE} --io ${IO} > test.out 

