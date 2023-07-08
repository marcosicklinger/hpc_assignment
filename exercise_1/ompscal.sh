#!/bin/bash

#SBATCH --job-name="weakmarco"
#SBATCH --output="log.out"
#SBATCH --partition=EPYC
#SBATCH	--nodes=3
#SBATCH --tasks-per-node=2
#SBATCH --ntasks=6
#SBATCH --exclusive
#SBATCH --cpus-per-task=64
#SBATCH --time=02:00:00

srun -n 1 make clean
srun -n 1 make game SAVINGFLAGS="-DTSAVE"

ntrials=10
time=100
for s in 10000 15000 20000
do
  for t in 1 2 4 8 16 32 48 64 96 128
  do
#    export OMP_PLACES=sockets
    export OMP_PROC_BIND=close
    export OMP_NUM_THREADS=$t
    for ((i=0; i<ntrials; i++))
    do
      mpirun --map-by socket -np $SLURM_NTASKS src/exe/game.x -e 1 -i -r -h $s -w $s -n $time >> time/ompscal.txt
    done
  done
done