hyperthread_mapping() { 

  if (( ${N_NODES} > 1  )); then 
    NUM_NODES=$((${N_NODES}/2))  
    END_CORES=${PPN}
  else
    NUM_NODES=${N_NODES} 
    END_CORES=$((${PPN}/2))
  fi 

  NUM_TASKS=$((${N_NODES}*${PPN}))  

  # Generate sequence such that the normal cores and HT cores(normal cores + 128) are stacked and seperated with a comma.   
  # seq 1 
  end=$((${END_CORES}-1))
  vals=($(seq 0 1 $(eval echo ${end})))

  # seq 2 
  end=$((${END_CORES}+128-1))
  start=128
  vals_HT=($(seq $(eval echo ${start}) 1 $(eval echo ${end})))
  
  unset updated # clear variable from before. 
  # shared mapping creates io and compute servers of neighbours.
  for ((i = 0; i < ${#vals[@]}; i++)); do
    updated+=(${vals[i]})
    updated+=(${vals_HT[i]})
  done

  # updated=("${vals[@]}" "${vals_HT[@]}")
  seq=$(IFS=, ; echo "${updated[*]}")
} 

hyperthread_mapping
