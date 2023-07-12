#!/bin/bash

#SBATCH --job-name="e1mpiomp"
#SBATCH --output="mpiomp.out"
#SBATCH --partition=EPYC
#SBATCH --nodes=4
#SBATCH --tasks-per-node=2
#SBATCH --ntasks=8
#SBATCH --exclusive
#SBATCH --time=02:00:00

module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1

srun -n 1 make clean_exe
srun -n 1 make game SAVINGFLAGS="-DTSAVE"

ntrials=2
nthreads=64
T=100
size=(7500 10606 12990 15000 16770 18371 19843 21213)

export OMP_PROC_BIND=close
export OMP_NUM_THREADS=$nthreads
for t in 1 2 3 4 5 6 7 8
do
#    export OMP_PLACES=sockets
    for ((i=0; i<ntrials; i++))
    do
      mpirun --map-by socket -np $t src/exe/game.x -e 1 -i -r -h ${size[$t]} -w ${size[$t]} -n $T >> time/mpiweak_multrials.txt
    done
done
