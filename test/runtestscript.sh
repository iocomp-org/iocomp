export TEST_HOME=TEST_OUTPUT
export STREAM_EXE='/home/shrey/Coding/iocomp/stream/test' 
export TEST_EXE='/home/shrey/Coding/iocomp/test/test' 

setup() {
	rm -rf ${TEST_DIR} 
	mkdir -p ${TEST_DIR} 
	cd ${TEST_DIR} 
} 

# MPI cores
CORES_START=2
CORES_END=4

# controls NX and NY parameters
ARRAY_START=1
ARRAY_END=3

# IO libraries selection 
IO_START=0
IO_END=2

# STREAM tests in HT flag 
echo 'Testing HT configuration'
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
			mpirun.mpich -n ${CORES} ${STREAM_EXE} --nx ${NX} --ny ${NY} --io ${IO} --HT 
			mpirun.mpich -n ${CORES} ${TEST_EXE} --nx ${NX} --ny ${NY} --io ${IO} 
		done 
	done
done 
