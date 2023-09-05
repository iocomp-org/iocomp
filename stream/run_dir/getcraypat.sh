for file in /mnt/lustre/a2fs-work3/work/e609/e609/shr203/iocomp/stream/run_dir/CRAY-PAT/*/*/*/*/*/*/*s
do
  path=$( echo ${file%/*} ) 
  pat_report -o cray_pat.out $file
  cp $(pwd)/cray_pat.out ${path}/cray_pat.out 
  rm $(pwd)/cray_pat.out
done

CURRENTDATE=`date +"%Y-%m-%d"` 
tar -cvf tars/CRAY_PAT_${CURRENTDATE}.tar.gz CRAY_PAT/*//*/*/*/*/*/*.csv CRAY_PAT/*/*/*/*/*/*/*.out 
