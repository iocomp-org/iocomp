export TEST_HOME='/home/shrey/Coding/iocomp/test/TEST_OUTPUT' 
export STREAM_EXE='/home/shrey/Coding/iocomp/stream/test' 
export CONFIG_FILE='/home/shrey/Coding/iocomp/stream/run_dir/config.xml' 
export TEST_EXE='/home/shrey/Coding/iocomp/test/test' 

setup() {
	rm -rf ${TEST_DIR} 
	mkdir -p ${TEST_DIR} 
	cd ${TEST_DIR} 
	cp ${CONFIG_FILE} . 
	# echo Testing in directory:; echo ${TEST_DIR} 
} 

weakscaling() {
	echo 'Testing ' ${FLAG} ' configuration'
	for CORES in $(seq ${CORES_START} 2 ${CORES_END});
	do 
		for i in $(seq ${ARRAY_START} ${ARRAY_END});
		do 
			NX=$((10**${i})) 
			NY=$((10**${i})) 
			if [ -n ${FLAG} ]; then 
				NX_TEST=$((${NX}/2))
			fi 
			if [ -z ${FLAG} ]; then 
				NX_TEST=${NX}
			fi 

			for IO in $(seq ${IO_START} ${IO_END});
			do	
				echo "Testing for ${CORES} cores, array size ${NX} x ${NY}, IO lib ${IO}"
				if [ -z ${FLAG} ]; then 
					export FLAG_DIR='NOSPLIT' 
				else
					export FLAG_DIR=${FLAG}
				fi 
				TEST_DIR=${TEST_HOME}/${FLAG_DIR}/${CORES}/${NX}/${NY}/${IO}
				echo DIRECTORY: $TEST_DIR
				setup
				mpirun.mpich -n ${CORES} ${STREAM_EXE} --nx ${NX} --ny ${NY} --io ${IO} --${FLAG} 
				echo "" 
				sleep 2
				# if flag activated then only half ranks are writing and global size is
				# also halved. 
				mpirun.mpich -n ${CORES} ${TEST_EXE} --nx ${NX_TEST} --ny ${NY} --io ${IO}
				echo "" 
				sleep 2
			done 
		done
	done 
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

echo "Testing no split case ..."
weakscaling

echo "Testing message copy case ..."
FLAG=HT
weakscaling 

echo "Testing shared case ..."
FLAG=shared
weakscaling 
