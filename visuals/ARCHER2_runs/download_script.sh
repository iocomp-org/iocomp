rm -rfi $1
mkdir $1
scp archer2:/work/e609/e609/shr203/iocomp/test/run_dir/$1/*.csv $1 
