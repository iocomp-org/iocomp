consecutive_nodesplit_mapping() { 

  NUM_NODES=${N_NODES} 
  NUM_TASKS=$((${N_NODES}*${PPN}))  
  END_CORES=${PPN}

  # Generate sequence such that nodesplit ordering is dividing same socket into 
  # I/O and Compute i.e. 0,2,4,..,126, 1,3,5...,127
  
  # seq 1 
  end=$((${END_CORES}-2))
  vals=($(seq 0 2 $(eval echo ${end})))

  # seq 2 
  end=$((${END_CORES}-1))
  vals2=($(seq 1 2 $(eval echo ${end})))
  
  updated=("${vals[@]}" "${vals2[@]}")
  seq=$(IFS=, ; echo "${updated[*]}")
} 

consecutive_nodesplit_mapping
