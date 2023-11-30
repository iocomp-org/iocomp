# bash script to call different kinds of scaling scripts for archer2 job submissions. 

callWeakScaling () {
  (
    export PPN=128
    export NX=$((2**11))
    export NY=$((2**11)) 
    export NODE_START=3
    export NODE_END=4
    export IO_START=0
    export IO_END=3
    export ARRAY="0"
    # export TIME="01:30:00"
    export MAP=0
    export CASE_START=1
    export CASE_END=5
    export DIR=CRAY_PAT/100COMPUTE/WEAK
    export MAP=0
    sh ./weakScaling.sh
  ) 
}

callStrongScaling () {
  (
    export PPN=128
    export NX=$((2**12))
    export NY=$((2**12)) 
    export NODE_START=5
    export NODE_END=5
    export IO_START=2
    export IO_END=2
    export ARRAY="0-2"
    export TIME="01:00:00"
    export CASE_START=4
    export CASE_END=5
    export MAP=0
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
    export MAP=0
    sh strongScaling.sh
  )
} 

callMAP() {
  (
    export PPN=128
    export NX=$((2**14))
    export NY=$((2**11)) 
    export NODE_START=0
    export NODE_END=0
    export ARRAY="0"
    export TIME="01:00:00"
    export DIR=MAP_PROFILES/100COMPUTE/STRONG # or weak 
    export MAP=1
    # loop over IO layers for MAP from 0 to 3 
    for io in $(seq 2 3)
    do 
      export IO_START=${io}
      export IO_END=${io} 
      # loop over cases 1 by 1 for MAP # from 1 - 5 
      for case in $(seq 1 5)
      do 
        export CASE_START=${case}
        export CASE_END=${case}
        # sh ./weakScaling.sh
        sh ./strongScaling.sh
      done 
    done 
  ) 
}

# callMAP
# callWeakScaling
callStrongScaling 
# callTest 
