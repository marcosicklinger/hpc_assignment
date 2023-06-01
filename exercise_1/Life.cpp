//
// Created by marcosicklinger on 5/21/23.
//

#include <cstring>
#include <cassert>
#include <iostream>
#include "Life.h"
#include "utils.h"
#include "consts.h"
#include <omp.h>
#include <mpi.h>

Life::Life(const std::string &filename, unsigned int &_rows, unsigned int &_cols):
name(filename), rows(_rows), cols(_cols), lifeSize(_rows*_cols) {

    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    lrank = (rank - 1 + n_procs) % n_procs;
    urank = (rank + 1) % n_procs;

    unsigned int local_size = lifeSize/n_procs;
    assert(local_size > 0);

    lo = local_size*rank;
    hi = local_size*(rank + 1);
    if (rank == n_procs - 1) {
        hi = lifeSize;
    }
    assert(lo < hi);

    localRows = rows/n_procs;
    localRowsHalo = localRows + 2;
    localColsHalo = cols + 2;
    localLifeSize = localRowsHalo * localColsHalo;
//    std::cout << localRowsHalo << " " << localColsHalo << std::endl;

    localState = new unsigned char [hi - lo];
    localObs = new unsigned char [localLifeSize];
    localObsNext = new unsigned char [localLifeSize];


    unsigned  char *globalState = read_state_from_pgm(filename); // should be done only on master process
    for (unsigned int i = 0; i < local_size; i++) {
        localState[i] = globalState[i]; // to be modified when parallelize code
//        std::cout << static_cast<int>(globalState[i]) << std::endl;
    }

    // in the case of multiple processes: add mp routine for sending and receiving local states

    for (unsigned int x = 0; x < localRows; x++) {
        for(unsigned int y = 0; y < cols; y++) {
            localObs[(x + 1) * localColsHalo + (y + 1)] = localState[x * cols + y];
//            std::cout << static_cast<int>(localObs[(x + 1) * localColsHalo + (y + 1)]) << std::endl;
        }
    }

    delete [] globalState;
}

Life::~Life() {
    delete [] localState;
    delete [] localObsNext;
    delete [] localObs;
}

void Life::computeHaloRows() {
    for (unsigned int y = 1; y <= cols; y++) {
        localObs[y] = localObs[localRows*localColsHalo + y];
        localObs[(localRowsHalo - 1)*localColsHalo + y] = localObs[localColsHalo + y];
    }
}

void Life::computeHaloCols() {
    for (unsigned int x = 1; x < localRows; x++) {
        localObs[x*localColsHalo] = localObs[x*localColsHalo + localColsHalo - 2];
        localObs[x*localColsHalo + localColsHalo - 1] = localObs[x*localColsHalo + 1];
    }
}

void Life::computeHaloCorners() {
    localObs[0] = localObs[rows * localColsHalo + localColsHalo - 2];
    localObs[localRowsHalo - 1] = localObs[rows * localColsHalo + 1];
    localObs[(localRowsHalo - 1)*localColsHalo] = localObs[2 * localColsHalo - 1];
    localObs[localRowsHalo * localColsHalo - 1] = localObs[localColsHalo + 1];
}

int Life::census(unsigned int &x, unsigned int &y) const {
    int dead = localObs[(x - 1)*localColsHalo + (y - 1)] + localObs[(x - 1)*localColsHalo + y] + localObs[(x - 1)*localColsHalo + (y + 1)] +
               localObs[x*localColsHalo + (y - 1)]                    + localObs[x*localColsHalo + (y + 1)] +
               localObs[(x + 1)*localColsHalo + (y - 1)] + localObs[(x + 1)*localColsHalo + y] + localObs[(x + 1)*localColsHalo + (y + 1)];
//    std::cout << static_cast<int>(localObs[(x + 1)*localColsHalo + (y - 1)]) << std::endl;
    return 8 - dead;
}

void Life::staticStep() {
    int it = 0;
    for (unsigned int x = 1; x <= localRows; x++) {
        for (unsigned int y = 1; y <= cols; y++){
            int local_population = census(x, y);
            bool lives = (localObs[x*localColsHalo + y] == ALIVE && (local_population == 2 || local_population == 3)) ||
                         (localObs[x*localColsHalo + y] == DEAD && local_population == 3);
            localObsNext[x*localColsHalo + y] = lives ? ALIVE : DEAD;
//            std::cout << it++ << " " << local_population << " " << lives << " " << static_cast<int>(localObs[x*localColsHalo + y]) << " " << static_cast<int>(localObsNext[x*localColsHalo + y]) << std::endl;
        }
    }
//    for (int i = 0; i < localColsHalo*localRowsHalo; i++) {
//            std::cout << static_cast<int>(localObs[i]) << std::endl;
//        }

}

void Life::orderedStep() {
    for (unsigned int x = 1; x <= localRows; x++) {
        for (unsigned int y = 1; y <= cols; y++) {
            int local_population = census(x, y);
            bool lives = (localObs[x*localColsHalo + y] == ALIVE && (local_population == 2 || local_population == 3)) ||
                         (localObs[x*localColsHalo + y] == DEAD && local_population == 3);
            localObs[x*localColsHalo + y] = lives ? ALIVE : DEAD;
//            std::cout << static_cast<int>(localObs[x*localColsHalo + y]) << std::endl;
            computeHaloRows();
            computeHaloCols();
            computeHaloCorners();
        }
    }
}

unsigned char *Life::getGlobalState() {
    // copy local observations (excluding halo data) into the local state for each process
    for (int x = 1; x <= localRows; x++) {
//        for (int y = 1; y <= cols; y++) {
//            localState[(x - 1)*cols + (y - 1)] = localObs[x*localColsHalo + y];
//        }
        std::memcpy(localState + (x - 1)*cols, localObs + x*localColsHalo + 1, cols*sizeof(unsigned char));
    }

    auto *globalState = new unsigned char[rows*cols];
    // for the other procs use mpi send/receive
    // copy master process' portion into global state array
    for (int x = 0; x < rows; x++) {
        std::memcpy(globalState + x*cols, localState + x*cols, cols*sizeof(unsigned char));
    }
    return globalState;
}

void Life::read_state(std::string &filename) {

}

void Life::staticEvolution(unsigned int &lifetime, unsigned int &record_every) {
    ++lifetime;
    for (int age = 1; age < lifetime; age++) {
        computeHaloRows();
        computeHaloCols();
        computeHaloCorners();
        staticStep();
        std::memcpy(localObs, localObsNext, localColsHalo*localRowsHalo*sizeof(unsigned char));
        if (age%record_every == 0) {
            freeze(age);
        }
    }
}

void Life::orderedEvolution(unsigned int &lifetime, unsigned int &record_every){
    ++lifetime;
    computeHaloRows();
    computeHaloCols();
    computeHaloCorners();
    for (int age = 1; age < lifetime; age++) {
        orderedStep();
        if (age%record_every == 0) {
                    freeze(age);
        }
    }
}

void Life::freeze(int &age) {
    auto *state = reinterpret_cast<const char*>(getGlobalState());
    std::string filename = name + std::to_string(age);
    write_state(filename, state, rows, cols);

    delete [] state;
}
