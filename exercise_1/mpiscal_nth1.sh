#!/bin/bash

#SBATCH --job-name="e1N"
#SBATCH --output="mpi1.out"
#SBATCH --partition=EPYC
#SBATCH --nodes=3
#SBATCH --exclusive
#SBATCH --time=02:00:00
#SBATCH --exclude=epyc002,epyc003

module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1

mpirun -n 1 make game SAVINGFLAGS="-DTSAVE"

ntrials=4
T=500

export OMP_NUM_THREADS=1

for s in 10000 15000
do
  for t in 32 64 128 192 256 320 384
  do
    for ((i=0; i<ntrials; i++))
    do
      mpirun -np $t --map-by core src/exe/game.x -e 1 -i -r -h $s -w $s -n $T -s 25 >> time/time_file.txt
    done
  done
done