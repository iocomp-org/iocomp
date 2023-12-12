#!/bin/bash 
set -eo pipefail # enable error handling in bash and quit bash when it fails 

# commmand line option gets directory name 
parent_directory=$1
base_directory=${parent_directory}
echo $parent_directory directory to test 

# get last directories, where the output files are going to be stored. 
# find $parent_directory -type d -links 2
# Get a list of end directories into an array
end_directories=($(find "$parent_directory" -type d -links 2))

# test for csv file
extension=".csv"

flag=1
# Check if files with the specified extension are not present in the last directory

# loop over end directories
for last_directory in "${end_directories[@]}"; do 
	if [ -z "$(find "${last_directory}" -maxdepth 1 -type f -name "*${extension}")" ]; then
    echo "No ${extension} files found in ${last_directory}"
		flag=0
	fi 
done 

if (( ${flag} == 1 )); then 
	echo "${extension} files are present in all relevant directories."
fi 


