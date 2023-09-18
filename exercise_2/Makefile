MKL = -L${MKLROOT}/lib/intel64  -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl
OPENBLASROOT=${OPENBLAS_ROOT}

PREC := -DUSE_FLOAT

# SETUP OF DIRECTORY
# ------------------
setup:
	mkdir -p sizes/single sizes/double
	mkdir -p cores/single cores/double
# ------------------

# EXECUTION
# ---------
executable:
	@chmod +x cores.sh
	@chmod +x sizes.sh
# ---------

# COMPILE ALL
# -----------
all: mkl.x oblas.x
# -----------

# COMPILE MKL
# -----------
mkl.x: gemm.cpp
	@g++ $(PREC) -DMKL $^ -m64 -I${MKLROOT}/include $(MKL) -o $@
#mkl.x: gemm.c
#	gcc -DUSE_FLOAT -DMKL $^ -m64 -I${MKLROOT}/include $(MKL) -o $@
# -----------

# COMPILE OBLAS
# -------------
oblas.x: gemm.cpp
	@g++ $(PREC) -DOPENBLAS $^ -m64 -I${OPENBLASROOT}/include -L/${OPENBLASROOT}/lib -lopenblas -lpthread -o $@ -fopenmp
#oblas.x: gemm.c
#	gcc -DUSE_FLOAT -DOPENBLAS $^ -m64 -I${OPENBLASROOT}/include -L/${OPENBLASROOT}/lib -lopenblas -lpthread -o $@ -fopenmp
# -------------

# CLEANING
# --------
.PHONY: clean

clean_exe:
	@rm -rf *.x
clean_cores:
	@rm -rf cores/double/*.txt
	@rm -rf cores/single/*.txt
clean_sizes:
	@rm -rf sizes/double/*.txt
	@rm -rf	sizes/single/*.txt
# --------