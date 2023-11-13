export RUNDIR=${PARENT_DIR}/${CASE}/${iter}
echo "**" $CASE 
echo $RUNDIR
rm -rf ${RUNDIR}
mkdir -p ${RUNDIR}
lfs setstripe -c -1  ${RUNDIR}
cd ${RUNDIR} 
cp ${CONFIG} . 
cp ${STREAM_MAKEOUTPUT} .
cp ${IOCOMP_MAKEOUTPUT} .

