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

    if (rank == 0) {
        auto *globalState = new int [lifeSize];
        read_state(filename, globalState, lifeSize);

        // master process takes the first chuck of rows
        std::memcpy(localState, globalState, (localSize)*sizeof(int));

        // sending of local data
        int offset = (rows%nTasks)*cols;
        for (int r = 1; r < nTasks; r++) {
            int add_offset = r == nTasks - 1 ? 1 : 0;
            MPI_Send(globalState + r*localSize, localSize + offset*add_offset,
                     MPI_INT, r, 0,
                     MPI_COMM_WORLD);
        }

        delete [] globalState;
    }
    // non-zero-rank processes receives local grids
    if (rank != 0) {
        MPI_Recv(localState, localSize,
                 MPI_INT,
                 0, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    MPI_Barrier(MPI_COMM_WORLD);

    // all the processes initialize their own padded sub-grids
    initializeObs();
}

Life::~Life() {
    delete [] localState;
    delete [] localObsNext;
    delete [] localObs;
}

void Life::computeHaloRows() {
    for (int y = 1; y <= cols; y++) {
        localObs[y] = localObs[localRows*localColsHalo + y];
        localObs[(localRowsHalo - 1)*localColsHalo + y] = localObs[localColsHalo + y];
    }
}

void Life::computeHaloCols() {
    for (int x = 0; x < localRowsHalo; x++) {
        localObs[x*localColsHalo] = localObs[x*localColsHalo + localColsHalo - 2];
        localObs[x*localColsHalo + localColsHalo - 1] = localObs[x*localColsHalo + 1];
    }
}

int Life::census(int x, int y) const {
    int dead = localObs[(x - 1)*localColsHalo + (y - 1)] + localObs[(x - 1)*localColsHalo + y] + localObs[(x - 1)*localColsHalo + (y + 1)] +
               localObs[x*localColsHalo + (y - 1)]                                                   + localObs[x*localColsHalo + (y + 1)] +
               localObs[(x + 1)*localColsHalo + (y - 1)] + localObs[(x + 1)*localColsHalo + y] + localObs[(x + 1)*localColsHalo + (y + 1)];
    return (8 - dead);
}

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

void Life::staticEvolution(int &lifetime, int &record_every) {
    ++lifetime;
    for (int age = 1; age < lifetime; age++) {
        haloExchange();
        computeHaloCols();
        staticStep();
        std::memcpy(localObs, localObsNext, localColsHalo*localRowsHalo*sizeof(int));
        #ifdef SSAVE
            if (age%record_every == 0) {
                freezeGlobalState(age);
            }
        #endif
    }
}

void Life::orderedEvolution(int &lifetime, int &record_every){
    try {
        if (nTasks > 1) {
            throw std::invalid_argument("To run the ordered evolution properly, the number of mpi processes must be equal to 1");
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
    }
    ++lifetime;
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

void Life::initializeObs(){
    for (int x = 0; x < localRows; x++) {
        for(int y = 0; y < cols; y++) {
            localObs[(x + 1) * localColsHalo + (y + 1)] = localState[x * cols + y];
        }
    }

}

void Life::haloExchange(){
    MPI_Status status1, status2;
    MPI_Sendrecv(localObs + (localRowsHalo - 2)*localColsHalo, localColsHalo, MPI_INT, upRank, 0,
                 localObs, localColsHalo, MPI_INT, loRank, 0,
                 MPI_COMM_WORLD, &status1);
    MPI_Sendrecv(localObs + localColsHalo, localColsHalo, MPI_INT, loRank, 1,
                 localObs + (localRowsHalo - 1)*localColsHalo, localColsHalo, MPI_INT, upRank, 1,
                 MPI_COMM_WORLD, &status2);
//    MPI_Barrier(MPI_COMM_WORLD);
}
