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

