#!/bin/bash

# Get the last directory in the current path
CWD=$(pwd)
last_directory=${CWD}/$1 #command line argument 
                         #with the parent directory to the map profiles needed

# Define the pattern you want to match
pattern="*.map"  # Change this to your desired pattern

# Use find to search for files in the last directory that match the pattern
files_array=($(find "$last_directory" -type f -name "$pattern"))

# Print the array elements (optional)
echo "List of files matching the pattern in the last directory:"
for file in "${files_array[@]}"; do

  echo "$file"
  # cd into directory where the file was found.
  file_directory=$(dirname "$file")
  cd ${file_directory}
  echo "cd-ing into directory " $file_directory
  file_name=$(basename "$file")
  echo "filename is " $file_name

  json_file="${file_name%.map}.json"
  echo "json file will be " ${json_file} 
  map --export=${json_file} ${file_name}
  wait 
done
