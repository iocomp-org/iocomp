# bash script to call different kinds of scaling scripts for archer2 job submissions. 

callWeakScaling () {
  (
    export PPN=128
    export NX=$((2**11))
    export NY=$((2**11)) 
    export NODE_START=0
    export NODE_END=0
    export IO_START=0
    export IO_END=0
    export ARRAY="0"
    export TIME="00:10:00"
    export DIR=OUTPUT/v2.0.0/WEAK/100COMPUTE
    sh ./weakScaling.sh
  ) 
}

callStrongScaling () {
  (
    export PPN=128
    export NX=$((2**14))
    export NY=$((2**11)) 
    export NODE_START=0
    export NODE_END=0
    export IO_START=0
    export IO_END=0
    export ARRAY="0"
    export TIME="00:10:00"
    export DIR=OUTPUT/v2.0.0/STRONG/GLOBALSIZE_8GB/100COMPUTE
    sh strongScaling.sh
  )
} 

callTest () {
  (
    export PPN=128
    export NX=$((2**14))
    export NY=$((2**11)) 
    export NODE_START=6
    export NODE_END=6
    export IO_START=0
    export IO_END=0
    export ARRAY="0"
    export TIME="00:10:00"
    export DIR=TESTING
    sh strongScaling.sh
  )
} 

callMAP() {
  (
    export PPN=128
    export NX=$((2**11))
    export NY=$((2**11)) 
    export NODE_START=0
    export NODE_END=0
    export IO_START=1
    export IO_END=1
    export ARRAY="0"
    export TIME="00:10:00"
    export DIR=MAP_PROFILES
    export MAP=1
    # loop over cases 1 by 1 for MAP 
    for case in $(seq 1 5)
    do 
      export CASE_START=${case}
      export CASE_END=${case}
      sh ./weakScaling.sh
    done 
  ) 
}

callMAP
# callWeakScaling
# callStrongScaling 
# callTest 
