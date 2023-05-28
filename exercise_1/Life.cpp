//
// Created by marcosicklinger on 5/21/23.
//

#include <cstring>
#include <cassert>
#include "Life.h"
#include "utils.h"

Life::Life(const std::string &filename, int &_rows, int &_cols):
name(filename), rows(_rows), cols(_cols), lifeSize(_rows*_cols) {

    unsigned int n_procs = 1;
    unsigned int local_size = lifeSize/n_procs;
    unsigned int rank = 0;

    lo = local_size*rank;
    hi = local_size*(rank + 1);
    assert(lo < hi);

    localRows = rows/n_procs;
    localRowsHalo = localRows + 2;
    localColsHalo = cols + 2;

    localState = new unsigned char [hi - lo];
    localObs = new unsigned char [localRowsHalo * localColsHalo];
    localObsNext = new unsigned char [hi - lo];

    unsigned  char *globalState = read_state_from_pgm(filename); // should be done only on master process
    for (size_t i = 0; i < local_size; i++) {
        localState[i] = globalState[i]; // to be modified when parallelize code
    }

    // in the case of multiple processes: add mp routine for sending and receiving local states

    for (size_t x = 0; x < localRows; x++) {
        for(size_t y = 0; y < cols; y++) {
            localObs[(x + 1) * localColsHalo + (y + 1)] = localState[x * cols + y];
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
    for (unsigned int x = 1; x < localRowsHalo - 1; x++) {
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
    unsigned int c = x*cols + y;
    unsigned int r_1 = c - cols;
    unsigned int r_2 = c + cols;
    return localObs[r_1 - 1] + localObs[r_1] + localObs[r_1 + 1] +
           localObs[c - 1]                    + localObs[c + 1] +
           localObs[r_2 - 1] + localObs[r_2] + localObs[r_2 + 1];
}

void Life::staticStep() {
    computeHaloCols();
    computeHaloCorners();
    for (unsigned int x = 1; x <= localRows; x++) {
        for (unsigned int y = 1; y <= cols; y++){
            int local_population = census(x, y);
            bool lives = (localObs[x*localColsHalo + y] == ALIVE && (local_population == 2 || local_population == 3)) ||
                         (localObs[x*localColsHalo + y] == DEAD && local_population == 3);
            localObsNext[x*localColsHalo + y] = lives ? ALIVE : DEAD;
        }
    }
    std::memcpy(localObs, localObsNext, localColsHalo*localRowsHalo*sizeof(unsigned char));
}

void Life::orderedStep() {
    for (unsigned int x = 1; x <= localRows; x++) {
        for (unsigned int y = 1; y <= cols; y++) {
            int local_population = census(x, y);
            bool lives = (localObs[x*localColsHalo + y] == ALIVE && (local_population == 2 || local_population == 3)) ||
                         (localObs[x*localColsHalo + y] == DEAD && local_population == 3);
            localObs[x*localColsHalo + y] = lives ? ALIVE : DEAD;
            computeHaloRows();
            computeHaloCols();
            computeHaloCorners();
        }
    }
}

unsigned char *Life::getGlobalState() {
    // copy local observations (excluding halo data) into the local state for each process
    for (int x = 1; x <= localRows; x++) {
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

void Life::staticEvolution(int &time, int record_every = 1) {
    for (int age = 0; age < time; age++) {
        staticStep();
        if (age%record_every == 0) {
            freeze(age);
        }
    }
}

void Life::orderedEvolution(int & time, int record_every){

}

void Life::freeze(int &age) {
    auto *state = reinterpret_cast<const char*>(getGlobalState());
    std::string filename = name + std::to_string(age);
    write_state(filename, state, rows, cols);

    delete [] state;
}
