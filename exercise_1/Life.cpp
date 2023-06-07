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
#include <mpi.h>

Life::Life(std::string location, std::string filename, int &_rows, int &_cols):
loc(std::move(location)), rows(_rows), cols(_cols), lifeSize(_rows*_cols) {

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
        localSize =localRows*cols;
        hi = lifeSize;
        hiRow = rows;
    }
//    std::cout << rank << " " << localRows << " " << std::endl;

    assert(lo < hi);
    assert(localSize > 0 && localSize == hi - lo);
    assert(loRow < hiRow);
    assert((hiRow - loRow)*cols == hi - lo);

    localRowsHalo = localRows + 2;
    localColsHalo = cols + 2;
    localSizeHalo = localRowsHalo * localColsHalo;

    localState = new unsigned char [localSize]();
    localObs = new unsigned char [localSizeHalo]();
    localObsNext = new unsigned char [localSizeHalo]();

    int offset = (rows%n_procs)*cols;
    if (rank == 0) {
        auto *globalState = new unsigned char [lifeSize];
        filename = loc + filename;
        read_state_from_pgm(globalState, filename);
//        std::cout << "here1" << std::endl;

        std::memcpy(localState, globalState, (hi - lo)*sizeof(unsigned char));

        for (int r = 1; r < n_procs; r++) {
//            std::cout << r << std::endl;
            int add_offset = r == n_procs - 1 ? 1 : 0;
            MPI_Send(globalState + r*localSize, hi - lo + offset*add_offset,
                     MPI_UNSIGNED_CHAR, r, 0,
                     MPI_COMM_WORLD);
        }
//        std::cout << "here2" << std::endl;
        // send to processes the various parts (in their local states)
        // then initializes their observations
        delete [] globalState;
//        std::cout << "here3" << std::endl;
    }
    if (rank != 0) {
        MPI_Recv(localState, hi - lo,
                 MPI_UNSIGNED_CHAR,
                 0, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
//    std::cout << "here4" << std::endl;
    initializeObs();
    MPI_Barrier(MPI_COMM_WORLD);
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
    #pragma omp parallel for schedule(static)
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

void Life::freezeGlobalState(int &age) {
    // copy local observations (excluding halo data) into the local state for each process
    for (int x = 1; x <= localRows; x++) {
        std::memcpy(localState + (x - 1)*cols, localObs + x*localColsHalo + 1, cols*sizeof(unsigned char));
    }

    unsigned char *globalState = nullptr;
    int *recv_counts = nullptr;
    int *displs = nullptr;

    if (rank == 0) {
        globalState = new unsigned char[rows*cols];
        recv_counts = new int[n_procs];
        displs = new int[n_procs];

        int offset = (rows%n_procs)*cols;
        for (int r = 0; r < n_procs; r++) {
            int add_offset = r == n_procs - 1 ? 1 : 0;
            recv_counts[r] = localSize + offset*add_offset;
            displs[r] = localSize*r;
        }
    }

    MPI_Gatherv(localState, localSize,
                MPI_UNSIGNED_CHAR,
                globalState, recv_counts, displs,
                MPI_UNSIGNED_CHAR,
                0,
                MPI_COMM_WORLD);

    if (rank == 0) {
        std::string filename = loc + std::to_string(age);
        write_state(filename, globalState, rows, cols);
    }

    delete [] globalState;
    delete [] recv_counts;
    delete [] displs;
}

void Life::read_state(std::string &filename) {

}

void Life::staticEvolution(int &lifetime, int &record_every) {
    ++lifetime;
    elapsed = -MPI_Wtime();
    for (int age = 1; age < lifetime; age++) {
        haloExchange();
        computeHaloCols();
//        computeHaloCorners();
        staticStep();
        std::memcpy(localObs, localObsNext, localColsHalo*localRowsHalo*sizeof(unsigned char));
        if (age%record_every == 0) {
            freezeGlobalState(age);
        }
    }
    elapsed += MPI_Wtime();
}

void Life::orderedEvolution(int &lifetime, int &record_every){
    ++lifetime;
    elapsed = -MPI_Wtime();
    computeHaloRows();
    computeHaloCols();
//    computeHaloCorners();
    for (int age = 1; age < lifetime; age++) {
        orderedStep();
        if (age%record_every == 0) {
            freezeGlobalState(age);
        }
    }
    elapsed += MPI_Wtime();
}

unsigned char& Life::operator[](int globalIdx){
    assert(globalIdx >= lo && globalIdx < hi);
    return localState[globalIdx - lo];
}

void Life::initializeObs(){
    for (int x = 0; x < localRows; x++) {
        for(int y = 0; y < cols; y++) {
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
    // process 0 and n_procs - 1 should exchange the necessary info for the corners too
    MPI_Barrier(MPI_COMM_WORLD);
}
