PPN=128 # tasks per node 

# WEAK SCALING  
for i in $(seq 0 0)
do 
  NUM_NODES=$(( 2**${i})) # 2^i nodes  
  DIR=v1.1.3/noMPItest/WEAK # name of starting directory with version of library 
  if ((x<3)) # different times for different number of nodes 
  then 
    TIME_VAR=4:00:00
  elif ((x==3))
  then 
    TIME_VAR=6:00:00
  else 
    TIME_VAR=8:00:00
  fi
  NX=4096
  NY=4096
  echo NODES ${NUM_NODES} PPN ${PPN} TIME ${TIME} ARRAY ${NX} x ${NY}  
  sbatch --export=ALL,NX=${NX},NY=${NY},DIR=${DIR} --qos=lowpriority --nodes=${NUM_NODES} --ntasks-per-node=${PPN} --array=0-2 --time=${TIME_VAR} --job-name=notest archer2.slurm 
done 

### STRONG SCALING 
#DIR=v1.1.3/STRONG
#for i in $(seq 0 3)
#do 
#  NUM_NODES=$(( 2**${i})) # 2^i nodes  
#  NX=4096
#  NY=$((${NX}/${NUM_NODES})) # for strong scaling, NY is reduced to maintain global size 
#  DIR=v1.1.3/STRONG
#  TIME_VAR=3:00:00
#  echo NODES ${NUM_NODES} PPN ${PPN} TIME ${TIME} ARRAY ${NX} x ${NY}  
#  sbatch --export=ALL,NX=${NX},NY=${NY},DIR=${DIR} --qos=lowpriority --nodes=${NUM_NODES} --ntasks-per-node=${PPN} --array=0-2 --time=${TIME_VAR} archer2.slurm 
#done 


### TEST
#for i in $(seq 0 0)
#do 
#  NUM_NODES=$(( 2**${i})) # 2^i nodes  
#  DIR=TEST
#  TIME_VAR=00:10:00
#  NX=128
#  NY=128
#  echo NODES ${NUM_NODES} PPN ${PPN} TIME ${TIME} ARRAY ${NX} x ${NY}  
#  sbatch --export=ALL,NX=${NX},NY=${NY},DIR=${DIR} --qos=lowpriority --nodes=${NUM_NODES} --ntasks-per-node=${PPN} --time=${TIME_VAR} archer2.slurm 
#done 
#
