#!/bin/bash

#SBATCH --job-name="e1mpi"
#SBATCH --output="log.out"
#SBATCH --partition=EPYC
#SBATCH	--nodes=3
#SBATCH --tasks-per-node=128
#SBATCH --ntasks=384
#SBATCH --exclusive
#SBATCH --time=03:00:00

srun -n 1 make clean
srun -n 1 make game SAVINGFLAGS="-DTSAVE"

ntrials=1
nthreads=1
time=100
for s in 7500 15000
do
  for t in 1 2 4 8 16 32 48 64 96 128 160 224 288 352 384
  do
#    export OMP_PLACES=sockets
    export OMP_PROC_BIND=close
    export OMP_NUM_THREADS=$nthreads
    for ((i=0; i<ntrials; i++))
    do
      mpirun --map-by core -np $t src/exe/game.x -e 1 -i -r -h $s -w $s -n $time >> time/static_omp.txt
    done
  done
done