setup:
	mkdir -p weak/single weak/double
	mkdir -p strong/single strong/double

MKL = -L${MKLROOT}/lib/intel64  -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl

OPENBLASROOT=${OPENBLAS_ROOT}

all: mkl.x oblas.x

mkl.x: gemm.cpp
	g++ -DUSE_FLOAT -DMKL $^ -m64 -I${MKLROOT}/include $(MKL) -o $@
#mkl.x: gemm.c
#	gcc -DUSE_FLOAT -DMKL $^ -m64 -I${MKLROOT}/include $(MKL) -o $@

oblas.x: gemm.cpp
	g++ -DUSE_FLOAT -DOPENBLAS $^ -m64 -I${OPENBLASROOT}/include -L/${OPENBLASROOT}/lib -lopenblas -lpthread -o $@ -fopenmp
#oblas.x: gemm.c
#	gcc -DUSE_FLOAT -DOPENBLAS $^ -m64 -I${OPENBLASROOT}/include -L/${OPENBLASROOT}/lib -lopenblas -lpthread -o $@ -fopenmp

clean:
	rm -rf *.x