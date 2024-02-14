#!/bin/bash
sequential_mapping() {

  if (( ${N_NODES} > 1  )); then 
    NUM_NODES=$((${N_NODES}/2))  
    END_CORES=${PPN}
    # half the number of total tasks divided between the allocated nodes.
    NUM_TASKS=$((${N_NODES}*${PPN}/2))  
  else 
    NUM_NODES=${N_NODES} 
    END_CORES=$((${PPN}/2))
    NUM_TASKS=$((${PPN}/2))  
  fi

  end=$((${END_CORES}-1))
  vals=($(seq 0 1 $(eval echo ${end})))
  seq=$(IFS=, ; echo "${vals[*]}")
}

sequential_mapping
