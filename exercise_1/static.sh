#!/bin/bash

ntrials=10
map="socket"
size="5000"
ntasks="1"
nthreads="1 2 4 8 9 10 11 12 16 32"
time=100
step=1
#fname=./snapshot/snapshot_00000

while getopts ":m:k:p:t:" opt; do
  case $opt in
    m) map=$OPTARG;;
    k) size=$OPTARG;;
    p) ntasks=$OPTARG;;
    t) nthreads=$OPTARG;;
    *) echo "invalid option";
        exit 1;;
  esac
done

make clean
make game SAVINGFLAGS="-DTSAVE -DSSAVE"

export OMP_PLACES=threads
export OMP_PROC_BIND=spread

for s in $size
do
  for p in $ntasks
  do
    for t in $nthreads
    do
      export OMP_NUM_THREADS=$t
      for ((i=0; i<ntrials; i++))
      do
        mpirun --map-by "$map" -np "$p" src/exe/game.x -e 1 -i -r -h "$s" -w "$s" -n $time -s $step >> time/static.txt
      done
    done
  done
done