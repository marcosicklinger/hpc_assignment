#!/bin/bash

#SBATCH --job-name="weakmarco"
#SBATCH --output="log.out"
#SBATCH --partition=EPYC
#SBATCH	--nodes=1
#SBATCH --exclusive
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=64
#SBATCH --time=02:00:00
#SBATCH --nodelist=epyc[005]

module load architecture/AMD
module load openBLAS/0.3.21-omp
module load mkl

cores="1 2 4 8 16 24 32 48 64"
ntrials=10
size=11000

srun -n 1 make clean
srun -n 1 make all PREC="-DUSE_FLOAT"

for c in $cores
do
  export OMP_NUM_THREADS=$c
  export OMP_PROC_BIND=close
  export OMP_PLACES=sockets
  for ((i=0; i<ntrials; i++))
  do
    srun -n 1 ./mkl.x  $size $size $size >> strong/mkl.txt
    srun -n 1 ./oblas.x  $size $size $size >> strong/oblas.txt
  done
done