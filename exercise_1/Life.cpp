//
// Created by marcosicklinger on 5/21/23.
//

#include <cstring>
#include <cassert>
#include "Life.h"
#include "utils.h"

Life::Life(std::string &filename, int &_rows, int &_cols): rows(_rows), cols(_cols), lifeSize(_rows*_cols) {

    int n_procs = 1;
    int local_size = lifeSize/n_procs;
    int rank = 0;

    lo = local_size*rank;
    hi = local_size*(rank + 1);
    assert(lo < hi);

    localRows = rows/n_procs;
    extendedLocalRows = localRows + 2;
    extendedLocalCols = cols + 2;

    globalState = read_state_from_pgm(filename); // should be done only on master process
    localState = new unsigned char [hi - lo];
    localObservation = new unsigned char [hi - lo];
    extendedLocalObservation = new unsigned char [extendedLocalRows * extendedLocalCols];

    for (size_t i = 0; i < local_size; i++) {
        localState[i] = globalState[i]; // to be modified when parallelize code
    }

    // in the case of multiple processes: add mp routine for sending and receiving local states

    for (size_t x = 0; x < localRows; x++) {
        for(size_t y = 0; y < cols; y++) {
            extendedLocalObservation[(x + 1) * extendedLocalCols + (y + 1)] = localObservation[x * cols + y];
        }
    }
}

Life::~Life() {
    delete [] globalState;
    delete [] localState;
    delete [] localObservation;
    delete [] extendedLocalObservation;
}

void Life::computeHaloCols() {
    for (size_t x = 1; x < extendedLocalRows - 1; x++) {
        extendedLocalObservation[x * extendedLocalCols] = extendedLocalObservation[x * extendedLocalCols + extendedLocalCols - 2];
        extendedLocalObservation[x * extendedLocalCols + extendedLocalCols - 1] = extendedLocalObservation[x * extendedLocalCols + 1];
    }
}

void Life::computeHaloCorners() {
    extendedLocalObservation[0] = extendedLocalObservation[rows * extendedLocalCols + extendedLocalCols - 2];
    extendedLocalObservation[extendedLocalRows - 1] = extendedLocalObservation[rows * extendedLocalCols + 1];
    extendedLocalObservation[(extendedLocalRows - 1) * extendedLocalCols] = extendedLocalObservation[2 * extendedLocalCols - 1];
    extendedLocalObservation[extendedLocalRows * extendedLocalCols - 1] = extendedLocalObservation[extendedLocalCols + 1];
}

int Life::living(int &_x, int &_y) const {
    int c = _x*cols + _y;
    int r_1 = c - cols;
    int r_2 = c + cols;
    return globalState[r_1 - 1] + globalState[r_1] + globalState[r_1 + 1] +
           globalState[c - 1]                    + globalState[c + 1] +
           globalState[r_2 - 1] + globalState[r_2] + globalState[r_2 + 1];
}

void Life::evolve() {}

void Life::read_state(std::string &filename) {

}