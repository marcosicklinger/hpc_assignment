#!/bin/bash

#SBATCH --job-name="mpiwN"
#SBATCH --output="mpiw.out"
#SBATCH --partition=EPYC
#SBATCH --nodes=2
#SBATCH --tasks-per-node=2
#SBATCH --exclusive
#SBATCH --time=02:00:00
#SBATCH --exclude=epyc002

module load architecture/AMD
module load openMPI/4.1.5/gnu/12.2.1

srun -n 1 make clean_exe
srun -n 1 make game SAVINGFLAGS="-DTSAVE"

ntrials=10
nthreads=64
T=500
size=(12500 17677 21650 25000 27950 30618)

export OMP_PLACES=cores
export OMP_PROC_BIND=close
for t in 1 2 3 4 5 6
do
    export OMP_NUM_THREADS=$nthreads
    for ((i=0; i<ntrials; i++))
    do
      mpirun --map-by socket -np $t src/exe/game.x -e 1 -i -r -h ${size[$((t-1))]} -w ${size[$((t-1))]} -n $T -s 25 >> time/time_file.txt
    done
done

