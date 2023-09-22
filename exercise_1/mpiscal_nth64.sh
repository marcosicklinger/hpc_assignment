#!/bin/bash

#SBATCH --job-name="e1N"
#SBATCH --output="mpi64.out"
#SBATCH --partition=EPYC
#SBATCH --nodes=3
#SBATCH --tasks-per-node=2
#SBATCH --exclusive
#SBATCH --time=02:00:00
#SBATCH --exclude=epyc003

module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1

mpirun -n 1 make clean_exe
mpirun -n 1 make game SAVINGFLAGS="-DTSAVE"

ntrials=10
nthreads=64
T=500

export OMP_PROC_BIND=close
export OMP_NUM_THREADS=$nthreads


for s in 10000 15000
do
  for t in 1 2 3 4 5 6
  do
    for ((i=0; i<ntrials; i++))
    do
      mpirun -np $t --map-by socket src/exe/game.x -e 1 -i -r -h $s -w $s -n $T -s 25 >> time/time_file.txt
    done
  done
done
