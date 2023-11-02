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
CORES_START=2
CORES_END=2

# controls NX and NY parameters
ARRAY_START=2
ARRAY_END=2

# IO libraries selection 
IO_START=0
IO_END=0

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
			sleep 2
			# if flag activated then only half ranks are writing and global size is
			# also halved. 
			if [ -n ${FLAG} ]; then 
				NX_TEST=$((${NX}/2))
			else
				NX_TEST=${NX}
			fi 
			mpirun.mpich -n ${CORES} ${TEST_EXE} --nx ${NX_TEST} --ny ${NY} --io ${IO}
			sleep 5
		done 
	done
done 
