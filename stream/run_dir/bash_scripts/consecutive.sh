consecutive_mapping() { 
  
  NUM_NODES=${N_NODES} 
  NUM_TASKS=$((${N_NODES}*${PPN}))  
  END_CORES=${PPN}
  # Generate sequence from 0 to PPN 
  end=$((${END_CORES}-1))
  vals=($(seq 0 1 $(eval echo ${end})))
  seq=$(IFS=, ; echo "${vals[*]}")

} 

consecutive_mapping 
