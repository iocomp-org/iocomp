CURRENTDATE=`date +"%Y-%m-%d"` 
#tar -cvf tars/$1_${CURRENTDATE}.tar.gz $1/*/*/*/*/*/*/*.csv $1/*/*/*/*/*/*/*.out
#tar -cvf tars/$1_${CURRENTDATE}.tar.gz $1
tar -cvf tars/CRAY-PAT_${CURRENTDATE}.tar.gz CRAY-PAT/*//*/*/*/*/*/*.csv CRAY-PAT/*/*/*/*/*/*/*.out 
# tar -cvf darshan_output.tar.gz DARSHAN/*/*/*/*/*/*.csv DARSHAN/*/*/*/*/*/*.out 


