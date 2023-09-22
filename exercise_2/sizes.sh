#!/bin/bash

#SBATCH --job-name="e2csw"
#SBATCH --output="sizes.out"
#SBATCH --partition=EPYC
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=12
#SBATCH --time=02:00:00

module load architecture/Intel
module load openBLAS/0.3.23-omp
module load mkl

export OMP_NUM_THREADS=64
export OMP_PLACES=cores
export OMP_PROC_BIND=spread

minsize=2000
maxsize=20000
step=2000
ntrials=5

# SINGLE
PREC="-DUSE_FLOAT"
OBLAS_FNAME=weak/single/oblas_csw.txt
MKL_FNAME=weak/single/mkl_csw.txt
srun -n 1 make clean_src
srun -n 1 make all PREC=$PREC

for((k=minsize; k<=maxsize; k+=step))
do
  for ((i=0; i<ntrials; i++))
  do
    srun -n 1 ./mkl.x  "$k" "$k" "$k" >> $MKL_FNAME
    srun -n 1 ./oblas.x  "$k" "$k" "$k" >> $OBLAS_FNAME
  done
done

# DOUBLE
PREC="-DUSE_DOUBLE"
OBLAS_FNAME=weak/double/oblas_csw.txt
MKL_FNAME=weak/double/mkl_csw.txt
srun -n 1 make clean_src
srun -n 1 make all PREC=$PREC

for((k=minsize; k<=maxsize; k+=step))
do
  for ((i=0; i<ntrials; i++))
  do
    srun -n 1 ./mkl.x  "$k" "$k" "$k" >> $MKL_FNAME
    srun -n 1 ./oblas.x  "$k" "$k" "$k" >> $OBLAS_FNAME
  done
done
