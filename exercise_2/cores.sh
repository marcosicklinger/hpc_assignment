#!/bin/bash

#SBATCH --job-name="e2scs"
#SBATCH --output="e2.out"
#SBATCH --partition=THIN
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=12
#SBATCH --time=02:00:00

module load architecture/Intel
module load openBLAS/0.3.23-omp
module load mkl

cores="1 2 3 4 6 8 10 12"
ntrials=5
size=11000

export OMP_PLACES=sockets
export OMP_PROC_BIND=close

# SINGLE
PREC="-DUSE_FLOAT"
OBLAS_FNAME=strong/single/oblas_scs.txt
MKL_FNAME=strong/single/mkl_scs.txt
srun -n 1 make clean_src
srun -n 1 make all PREC=$PREC

for c in $cores
do
  export OMP_NUM_THREADS=$c
  for ((i=0; i<ntrials; i++))
  do
    srun -n 1 ./mkl.x  $size $size $size >> $MKL_FNAME
    srun -n 1 ./oblas.x  $size $size $size >> $OBLAS_FNAME
  done
done

# DOUBLE
PREC="-DUSE_DOUBLE"
OBLAS_FNAME=strong/double/oblas_scs.txt
MKL_FNAME=strong/double/mkl_scs.txt
srun -n 1 make clean_src
srun -n 1 make all PREC=$PREC

for c in $cores
do
  export OMP_NUM_THREADS=$c
  for ((i=0; i<ntrials; i++))
  do
    srun -n 1 ./mkl.x  $size $size $size >> $MKL_FNAME
    srun -n 1 ./oblas.x  $size $size $size >> $OBLAS_FNAME
  done
done
