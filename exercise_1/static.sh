#!/bin/bash

ntrials=10
size="1000 2000 3000"
ntasks="1"
nthreads="1 2 4 8 16"
time=100
step=1
fname=0

for s in $size
do
  for p in $ntasks
  do
    for t in $nthreads
    do
      export OMP_NUM_THREADS=$t
      export OMP_PROC_BIND=close
      for ((i=0; i<ntrials; i++))
      do
        mpirun --map-by socket -np $p src/game.x -e 1 -i -r -h "$s" -w "$s" -f $fname -n $time -s $step >> time/static.txt
      done
    done
  done
done