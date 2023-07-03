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

export MKL_VERBOSE=0

cores="1 2"
ntrials=2
size=11000
PREC="-DUSE_FLOAT"
OBLAS_FNAME=""
MKL_FNAME=""
if [[ "$PREC" == "-DUSE_FLOAT" ]]; then
    OBLAS_FNAME=strong/single/oblas.txt
    MKL_FNAME=strong/single/mkl.txt
    echo "precision is set to -DUSE_FLOAT"
elif [[ "$PREC" == "-DUSE_DOUBLE" ]]; then
    OBLAS_FNAME=strong/double/oblas.txt
    MKL_FNAME=strong/single/mkl.txt
    echo "precision is set to -DUSE_DOUBLE"
else
    echo "Invalid precision flag. Stopping."
    exit 1
fi
echo "Starting..."

make clean
make all PREC="$PREC"

for c in $cores
do
  echo "number of cores: $c"
  export OMP_NUM_THREADS=$c
  export OMP_PROC_BIND=close
  export OMP_PLACES=sockets
  for ((i=0; i<ntrials; i++))
  do
    echo "  trial number: $((i+1))"
    echo "    MKL"
    ./mkl.x  $size $size $size >> $MKL_FNAME
    echo "    openBLAS"
    ./oblas.x  $size $size $size >> $OBLAS_FNAME
  done
done