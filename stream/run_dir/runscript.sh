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

# callWeakScaling
# callStrongScaling 
callTest 
