rm -rf ${WORKING_DIR}
mkdir -p ${WORKING_DIR}
lfs setstripe -c -1  ${WORKING_DIR}
cd ${WORKING_DIR} 
cp ${CONFIG} . 

# check if make output files from stream and iocomp, and if yes copy them into working directory.
if [ -f ${STREAM_MAKEOUTPUT} ]; then 
  cp ${STREAM_MAKEOUTPUT} .
else 
  echo 'stream make output not available' 
fi 

if [ -f ${IOCOMP_MAKEOUTPUT} ]; then 
  cp ${IOCOMP_MAKEOUTPUT} .
else 
  echo 'iocomp make output not available' 
fi 
  

