#include <cstring>
#include <stdexcept>
#include <mpi.h>
#include <ostream>
#include <iostream>
#include "../include/Life.h"
#include "../include/utils.h"
#include "../include/consts.h"


Life::Life(const std::string &filename, int &_rows, int &_cols, int np, int rk):
nTasks(np),
rank(rk),
rows(_rows),
cols(_cols),
lifeSize(_rows*_cols) {

    // computation of upper and lower processes' ranks
    loRank = (rank - 1 + nTasks) % nTasks;
    upRank = (rank + 1) % nTasks;

    // computation of the rows for each process' sub-grid
    localRows = rows/nTasks;
    localSize = localRows * cols;
    // last process takes the remaining rows
    if (rank == nTasks - 1) {
        localRows += rows%nTasks;
        localSize = localRows*cols;
    }

    // computation of rows and columns for padded sub-grids
    localRowsHalo = localRows + 2;
    localColsHalo = cols + 2;
    localSizeHalo = localRowsHalo * localColsHalo;

    // allocation for the local grid, local padded grid
    // local padded grid at successive time step
    localState = new int [localSize]();
    localObs = new int [localSizeHalo]();
    localObsNext = new int [localSizeHalo]();

    // initialize buffers and displacements
    int *globalState = nullptr;
    int *send_counts = nullptr;
    int *displs = nullptr;

    // master process computes chunks' information to send
    // to the other processes
    if (rank == 0) {
        globalState = new int[rows*cols];
        read_state(filename, globalState, lifeSize);

        send_counts = new int[nTasks];
        displs = new int[nTasks];

        int offset = (rows%nTasks)*cols;
        for (int r = 0; r < nTasks; r++) {
            int add_offset = r == nTasks - 1 ? 1 : 0;
            send_counts[r] = localSize + offset*add_offset;
            displs[r] = localSize*r;
        }
    }

    // scatter all sub-grids to all processes
    MPI_Scatterv(globalState, send_counts, displs,
                 MPI_INT,
                 localState, localSize,
                 MPI_INT,
                 0,
                 MPI_COMM_WORLD);

    delete [] globalState;
    delete [] send_counts;
    delete [] displs;

    // all the processes initialize their own padded sub-grids
    initializeObs();
}

Life::~Life() {
    delete [] localState;
    delete [] localObsNext;
    delete [] localObs;
}

// (naive) computation of padded columns
void Life::computeHaloRows() {
    for (int y = 1; y <= cols; y++) {
        localObs[y] = localObs[localRows*localColsHalo + y];
        localObs[(localRowsHalo - 1)*localColsHalo + y] = localObs[localColsHalo + y];
    }
}

// computation of padded columns
void Life::computeHaloCols() {
    for (int x = 0; x < localRowsHalo; x++) {
        localObs[x*localColsHalo] = localObs[x*localColsHalo + localColsHalo - 2];
        localObs[x*localColsHalo + localColsHalo - 1] = localObs[x*localColsHalo + 1];
    }
}

// computation of the alive neighbors
int Life::census(int x, int y) const {
    auto x_left = x - 1;
    auto x_right = x + 1;
    auto y_up = y - 1;
    auto y_down = y + 1;

    auto nb1 = localObs[x_left*localColsHalo + y_up];
    auto nb2 = localObs[x_left*localColsHalo + y];
    auto nb3 = localObs[x_left*localColsHalo + y_down];
    auto nb4 = localObs[x*localColsHalo + y_up];
    auto nb5 = localObs[x*localColsHalo + y_down];
    auto nb6 = localObs[x_right*localColsHalo + y_up];
    auto nb7 = localObs[x_right*localColsHalo + y];
    auto nb8 = localObs[x_right*localColsHalo + y_down];
    auto dead = nb1 + nb2 + nb3 + nb4 + nb5 + nb6 + nb7 + nb8;

    return (8 - dead);
}

// single step of static evolution
void Life::staticStep() {
    #pragma omp parallel for schedule(static) collapse(2)
    for (int x = 1; x <= localRows; x++) {
        for (int y = 1; y <= cols; y++){
            int local_population = census(x, y);
            bool lives = (localObs[x*localColsHalo + y] == ALIVE && (local_population == 2 || local_population == 3)) ||
                         (localObs[x*localColsHalo + y] == DEAD && local_population == 3);
            localObsNext[x*localColsHalo + y] = lives ? ALIVE : DEAD;
        }
    }
}

// single step of ordered evolution
void Life::orderedStep() {
    for (int x = 1; x <= localRows; x++) {
        for (int y = 1; y <= cols; y++) {
            int local_population = census(x, y);
            bool lives = (localObs[x*localColsHalo + y] == ALIVE && (local_population == 2 || local_population == 3)) ||
                         (localObs[x*localColsHalo + y] == DEAD && local_population == 3);
            localObs[x*localColsHalo + y] = lives ? ALIVE : DEAD;
            computeHaloRows();
            computeHaloCols();
        }
    }
}

// system saving through utility functions
void Life::freezeGlobalState(int &age) {
    for (int x = 1; x <= localRows; x++) {
        std::memcpy(localState + (x - 1)*cols, localObs + x*localColsHalo + 1, cols*sizeof(int));
    }

    int *globalState = nullptr;
    int *recv_counts = nullptr;
    int *displs = nullptr;

    if (rank == 0) {
        globalState = new int[rows*cols];
        recv_counts = new int[nTasks];
        displs = new int[nTasks];

        int offset = (rows%nTasks)*cols;
        for (int r = 0; r < nTasks; r++) {
            int add_offset = r == nTasks - 1 ? 1 : 0;
            recv_counts[r] = localSize + offset*add_offset;
            displs[r] = localSize*r;
        }
    }

    MPI_Gatherv(localState, localSize,
                MPI_INT,
                globalState, recv_counts, displs,
                MPI_INT,
                0,
                MPI_COMM_WORLD);

    if (rank == 0) {
        std::string age_string = std::to_string(age);
        pad_age_string(age_string);
        std::string filename = FNAME_PREFIX + age_string;
        write_state(filename, globalState, rows, cols);
    }

    delete [] globalState;
    delete [] recv_counts;
    delete [] displs;

}

// static evolution
void Life::staticEvolution(int &lifetime, int &record_every) {
    for (int age = 1; age < lifetime; age++) {
        haloExchange();
        computeHaloCols();
        staticStep();
        int *swap = localObsNext;
        localObsNext = localObs;
        localObs = swap;
        #ifdef SSAVE
            if (age%record_every == 0 || age == lifetime) {
                freezeGlobalState(age);
            }
        #endif
    }
    freezeGlobalState(lifetime);
}

// ordered evolution
void Life::orderedEvolution(int &lifetime, int &record_every){
    computeHaloRows();
    computeHaloCols();
    for (int age = 1; age < lifetime; age++) {
        orderedStep();
        #ifdef SSAVE
            if (age%record_every == 0) {
                freezeGlobalState(age);
            }
        #endif
    }
}

// initialize observations' buffers
void Life::initializeObs(){
    #pragma omp parallel for schedule(static)
    for (int x = 0; x < localRows; x++) {
        for(int y = 0; y < cols; y++) {
            localObs[(x + 1) * localColsHalo + (y + 1)] = localState[x * cols + y];
        }
    }

}

//  implementation of message passing approach
void Life::haloExchange(){
    MPI_Sendrecv(localObs + rows*localColsHalo, localColsHalo, MPI_INT, upRank, 0,
                 localObs, localColsHalo, MPI_INT, loRank, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Sendrecv(localObs + localColsHalo, localColsHalo, MPI_INT, loRank, 1,
                 localObs + (localRowsHalo - 1)*localColsHalo, localColsHalo, MPI_INT, upRank, 1,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
