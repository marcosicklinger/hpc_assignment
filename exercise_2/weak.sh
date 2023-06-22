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

#module load architecture/AMD
#module load openBLAS/0.3.21-omp
#module load mkl

export OMP_NUM_THREADS=64
export OMP_PROC_BIND=close
export OMP_PLACES=cores

minsize=2000
maxsize=20000
step=2000
ntrials=10

p="-DUSE_FLOAT"

make clean
make all PREC=$p

#for((k=minsize; k<=maxsize; k+=step))
#do
#  for((i=0; i<ntrials; i++))
#  do
#    srun -n1 ./mkl.x "$k" "$k" $k >> t
#    srun -n ./olas.x "$k" "$k" "$k" >> t
#  done
#done