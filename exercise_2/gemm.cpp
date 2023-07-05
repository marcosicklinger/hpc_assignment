/*
 * authors   : Giuseppe Piero Brandino - eXact-lab s.r.l.
 * date      : October 2018
 * copyright : GNU Public License
 * modifiedby: Stefano Cozzini for DSSC usage
 */

#include <ctime>
#include <unistd.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <iomanip>

#define min(x,y) (((x) < (y)) ? (x) : (y))

#ifdef MKL
#include "mkl_cblas.h"
#endif

#ifdef OPENBLAS
#include "cblas.h"
#endif

#ifdef BLIS
#include "cblas.h"
#endif

#ifdef USE_FLOAT
#define MYFLOAT float
#define DATATYPE //printf(" Using float \n\n");
#define GEMMCPU cblas_sgemm
#endif

#ifdef USE_DOUBLE
#define MYFLOAT double
#define DATATYPE //printf(" Using double \n\n");
#define GEMMCPU cblas_dgemm
#endif

struct timespec diff(struct timespec start, struct timespec end)
{
        struct timespec temp{};
        if ((end.tv_nsec-start.tv_nsec)<0) {
                temp.tv_sec = end.tv_sec-start.tv_sec-1;
                temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
        } else {
                temp.tv_sec = end.tv_sec-start.tv_sec;
                temp.tv_nsec = end.tv_nsec-start.tv_nsec;
        }
        return temp;
}


int main(int argc, char** argv)
{
    MYFLOAT *A, *B, *C;
    int m, n, k, i, j;
    MYFLOAT alpha, beta;
    struct timespec begin{}, end{};
    double elapsed;
    if (argc == 1)
    {
    m = 2000, k = 200, n = 1000;
    }
    else if (argc == 4)
    {
        m = atoi(argv[1]);
        k = atoi(argv[2]);
        n = atoi(argv[3]);
    }
    else
    {
    std::cout << "Usage: " << argv[0] << " M K N, the corresponding matrices will be A(M,K) B(K,N)" << std::endl;
    return 0;
    }

//    std::cout << "\nThis example computes real matrix C=alpha*A*B+beta*C using\n"
//                 "BLAS function dgemm, where A, B, and C are matrices and\n"
//                 "alpha and beta are scalars\n\n"
//                 "Initializing data for matrix multiplication C=A*B for matrix\n"
//                 "A(" << m << "x" << k << ") and matrix B(" << k << "x" << n << ")\n\n"
//    << std::endl;

    DATATYPE
    alpha = 1.0; beta = 0.0;

    A = new MYFLOAT [m*k];
    B = new MYFLOAT [k*n];
    C = new MYFLOAT [m*n];

    if (A == NULL || B == NULL || C == NULL) {
      std::cout << "\n ERROR: Can't allocate memory for matrices. Aborting... \n\n" << std::endl;
      delete [] A;
      delete [] B;
      delete [] C;
      return 1;
    }

    for (i = 0; i < (m*k); i++) {
        A[i] = (MYFLOAT)(i+1);
    }

    for (i = 0; i < (k*n); i++) {
        B[i] = (MYFLOAT)(-i-1);
    }

    for (i = 0; i < (m*n); i++) {
        C[i] = 0.0;
    }

    sleep(1);
//    std::cout << " Computing matrix product using gemm function via CBLAS interface \n" << std::endl;
    clock_gettime(CLOCK_MONOTONIC, &begin);
    GEMMCPU(CblasColMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A, m, B, k, beta, C, m);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = (double)diff(begin,end).tv_sec + (double)diff(begin,end).tv_nsec / 1000000000.0;
    double gflops = 2.0 * m *n*k;
    gflops = gflops/elapsed*1.0e-9;
//    std::cout << "\nElapsed time " << diff(begin, end).tv_sec << "." << diff(begin, end).tv_nsec << " s\n\n\n";
    // output: m - n - k - elapsed (sec) - gflops
    std::cout << m << "\t" << n << "\t" << k << "\t" << elapsed << "\t" << gflops << std::endl;


    #ifdef PRINT
        std::cout << " Top left corner of matrix A: \n";
        for (i=0; i<min(m,6); i++) {
          for (j=0; j<min(k,6); j++) {
            std::cout << std::fixed << std::setw(12) << std::setprecision(0) << A[j+i*k];
          }
          std::cout << "\n";
        }

        std::cout << "\n Top left corner of matrix B: \n";
        for (i=0; i<min(k,6); i++) {
          for (j=0; j<min(n,6); j++) {
            std::cout << std::fixed << std::setw(12) << std::setprecision(0) << B[j+i*n];
          }
          printf ("\n");
        }

        std::cout << "\n Top left corner of matrix C: \n";
        for (i=0; i<min(m,6); i++) {
          for (j=0; j<min(n,6); j++) {
            std::cout << std::fixed << std::setw(12) << std::setprecision(0) << C[j+i*n];
          }
          printf ("\n");
        }
    #endif

    delete [] A;
    delete [] B;
    delete [] C;

    return 0;
}
