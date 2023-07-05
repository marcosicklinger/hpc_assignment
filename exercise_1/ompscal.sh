#!/bin/bash

nodes=3
tpn=2
salloc -p EPYC --nodes $nodes --tasks-per-node $tpn --time=03:00:00

ntrials=10
size="10000 15000 20000"
nthreads="1 2 4 8 16 32 48 64 96 128"
time=100

make clean
make game SAVINGFLAGS="-DTSAVE"

ntasks=$((nodes*tpn))
for s in $size
do
  for t in $nthreads
  do
#    export OMP_PLACES=sockets
    export OMP_PROC_BIND=close
    export OMP_NUM_THREADS=$t
    for ((i=0; i<ntrials; i++))
    do
      mpirun --map-by socket -np $ntasks src/exe/game.x -e 1 -i -r -h "$s" -w "$s" -n $time >> time/static_omp.txt
    done
  done
done