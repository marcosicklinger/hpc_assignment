#!/bin/bash

ntrials=10
size="100 200"
time=100
step=1
fname=0

while getopts ":k:" opt; do
  case $opt in
    k) size=$OPTARG;;
    *) echo "invalid option";
        exit 1;;
  esac
done

make clean
make game SAVINGFLAGS="-DTSAVE"

export OMP_NUM_THREADS=1
for s in $size
do
  for ((i=0; i<ntrials; i++))
  do
    mpirun -np 1 src/game.x -e 0 -i -r -h "$s" -w "$s" -f $fname -n $time -s $step >> time/ordered.txt
  done
done