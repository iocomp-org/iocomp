export TEST_HOME='/home/shrey/Coding/iocomp/test/TEST_OUTPUT' 
export STREAM_EXE='/home/shrey/Coding/iocomp/stream/test' 
export CONFIG_FILE='/home/shrey/Coding/iocomp/stream/run_dir/config.xml' 
export TEST_EXE='/home/shrey/Coding/iocomp/test/test' 

setup() {
	rm -rf ${TEST_DIR} 
	mkdir -p ${TEST_DIR} 
	cd ${TEST_DIR} 
	cp ${CONFIG_FILE} . 
	echo Testing in directory:; echo ${TEST_DIR} 
} 

# MPI cores
CORES_START=4
CORES_END=4

# controls NX and NY parameters
ARRAY_START=2
ARRAY_END=2

# IO libraries selection 
IO_START=0
IO_END=3

# flag
FLAG=HT
# FLAG=shared

# STREAM tests in HT flag 
echo 'Testing ' ${FLAG} ' configuration'
for CORES in $(seq ${CORES_START} 2 ${CORES_END});
do 
	for i in $(seq ${ARRAY_START} ${ARRAY_END});
	do 
		NX=$((10**${i})) 
		NY=$((10**${i})) 
		for IO in $(seq ${IO_START} ${IO_END});
		do	
			echo "Testing for ${CORES} cores, array size ${NX} x ${NY}, IO lib ${IO}"
			TEST_DIR=${TEST_HOME}/${CORES}/${NX}/${NY}/${IO}
			setup
			mpirun.mpich -n ${CORES} ${STREAM_EXE} --nx ${NX} --ny ${NY} --io ${IO} --${FLAG} 
			sleep 5
			echo testing in; echo ${TEST_DIR}
			# if flag activated then only half ranks are writing and global size is
			# also halved. 
			if [ -n ${FLAG} ]; then 
				TEST_CORES=$((${CORES}/2))
			else
				TEST_CORES=${CORES}
			fi 
			mpirun.mpich -n ${TEST_CORES} ${TEST_EXE} --nx ${NX} --ny ${NY} --io ${IO}
			sleep 5
		done 
	done
done 
