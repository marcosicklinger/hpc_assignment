#!/bin/bash

#SBATCH --job-name="e1omp"
#SBATCH --output="omp.out"
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --exclusive
#SBATCH --time=02:00:00

module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1

mpirun -np 1 make clean_exe
mpirun -np 1 make game SAVINGFLAGS="-DTSAVE"

ntrials=4
T=500

export OMP_PLACES=cores
export OMP_PROC_BIND=close
for s in 20000
do
  for t in 1 4 8 16 32 48 64
  do
    export OMP_NUM_THREADS=$t
    for ((i=0; i<ntrials; i++))
    do
      mpirun -np 1 --map-by socket src/exe/game.x -e 1 -i -r -h $s -w $s -n $T -s 100 >> time/time_file.txt
    done
  done
done
