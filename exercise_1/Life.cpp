//
// Created by marcosicklinger on 5/21/23.
//

#include <cstring>
#include <cassert>
#include <iostream>
#include <utility>
#include "Life.h"
#include "utils.h"
#include "consts.h"
#include <omp.h>
#include <mpi.h>

Life::Life(std::string location, std::string filename, int &_rows, int &_cols):
loc(std::move(location)), rows(_rows), cols(_cols), lifeSize(_rows*_cols) {

    int n_procs;
    int rank;
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    loRank = (rank - 1 + n_procs) % n_procs;
    upRank = (rank + 1) % n_procs;
//    std::cout << rank << " " << loRank << " " << upRank << std::endl;

    localRows = rows/n_procs;
    localSize = localRows * cols;
    lo = localSize*rank;
    hi = localSize*(rank + 1);
    loRow = localRows*rank;
    hiRow = localRows*(rank + 1);
    if (rank == n_procs - 1) {
        localRows += rows%n_procs;
        hi = lifeSize;
        hiRow = rows;
    }
//    std::cout << rank << " " << localRows << " " << std::endl;

    assert(localSize > 0);
    assert(lo < hi);
    assert(loRow < hiRow);
    assert((hiRow - loRow)*cols == hi - lo);

    localRowsHalo = localRows + 2;
    localColsHalo = cols + 2;
    localSizeHalo = localRowsHalo * localColsHalo;

    localState = new unsigned char [localSize];
    localObs = new unsigned char [localSizeHalo];
    localObsNext = new unsigned char [localSizeHalo];

    if (rank == 0) {
        auto *globalState = new unsigned char [lifeSize];
        filename = loc + filename;
        read_state_from_pgm(globalState, filename);

        // send to processes the various parts (in their local states)
        // then initializes their observations

        initialize(globalState);
        delete [] globalState;
    }
//        for (int r = 1; r < n_procs; r++) {
//            MPI_Send(globalState + r * localSize, localSize, MPI_UNSIGNED_CHAR, r, 0, MPI_COMM_WORLD);
//        delete [] globalState;
//    }

    // in the case of multiple processes: add mp routine for sending and receiving local states
//
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
    for (unsigned int x = 0; x < localRowsHalo; x++) {
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

unsigned char Life::census(int x, int y) const {
    unsigned char dead = localObs[(x - 1)*localColsHalo + (y - 1)] + localObs[(x - 1)*localColsHalo + y] + localObs[(x - 1)*localColsHalo + (y + 1)] +
               localObs[x*localColsHalo + (y - 1)]                    + localObs[x*localColsHalo + (y + 1)] +
               localObs[(x + 1)*localColsHalo + (y - 1)] + localObs[(x + 1)*localColsHalo + y] + localObs[(x + 1)*localColsHalo + (y + 1)];
//    std::cout << static_cast<int>(localObs[(x + 1)*localColsHalo + (y - 1)]) << std::endl;
    return (8 - dead);
}

void Life::staticStep() {
    for (int x = 1; x <= localRows; x++) {
        for (int y = 1; y <= cols; y++){
            unsigned char local_population = census(x, y);
            bool lives = (localObs[x*localColsHalo + y] == ALIVE && (local_population == 2 || local_population == 3)) || (localObs[x*localColsHalo + y] == DEAD && local_population == 3);
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
//            std::cout << static_cast<int>(localObs[x*localColsHalo + y]) << std::endl;
            computeHaloRows();
            computeHaloCols();
//            computeHaloCorners();
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

void Life::staticEvolution(int &lifetime, int &record_every) {
    ++lifetime;
    for (int age = 1; age < lifetime; age++) {
        haloExchange();
        computeHaloCols();
        computeHaloCorners();
        staticStep();
        std::memcpy(localObs, localObsNext, localColsHalo*localRowsHalo*sizeof(unsigned char));
        if (age%record_every == 0) {
            freeze(age);
        }
    }
}

void Life::orderedEvolution(int &lifetime, int &record_every){
    ++lifetime;
    computeHaloRows();
    computeHaloCols();
//    computeHaloCorners();
    for (int age = 1; age < lifetime; age++) {
        orderedStep();
        if (age%record_every == 0) {
                    freeze(age);
        }
    }
}

void Life::freeze(int &age) {
    auto *state = getGlobalState();
    std::string filename = loc + std::to_string(age);
    write_state(filename, state, rows, cols);

    delete [] state;
}

unsigned char& Life::operator[](int globalIdx){
    assert(globalIdx >= lo && globalIdx < hi);
    return localState[globalIdx - lo];
}

void Life::initializeObs(){
    for (unsigned int x = 0; x < localRows; x++) {
        for(unsigned int y = 0; y < cols; y++) {
            localObs[(x + 1) * localColsHalo + (y + 1)] = localState[x * cols + y];
        }
    }

}

void Life::initialize(void *data){
    std::memcpy(localState, reinterpret_cast<unsigned char*>(data) + loRow*cols, (hi - lo)*sizeof(unsigned char));
    initializeObs();
}

void Life::haloExchange (){
    ompi_status_public_t status1, status2;
    MPI_Sendrecv(localObs + (localRowsHalo - 2)*localColsHalo, localColsHalo, MPI_UNSIGNED_CHAR, upRank, 0,
                 localObs, localColsHalo, MPI_UNSIGNED_CHAR, loRank, 0,
                 MPI_COMM_WORLD, &status1);
    MPI_Sendrecv(localObs + localColsHalo, localColsHalo, MPI_UNSIGNED_CHAR, loRank, 1,
                 localObs + (localRowsHalo - 1)*localColsHalo, localColsHalo, MPI_UNSIGNED_CHAR, upRank, 1,
                 MPI_COMM_WORLD, &status2);
}
