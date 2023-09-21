#include <cstring>
#include <stdexcept>
#include "../include/SerialLife.h"
#include "../include/utils.h"
#include "../include/consts.h"

SerialLife::SerialLife(const std::string &filename, int &_rows, int &_cols) :
rows(_rows),
cols(_cols),
lifeSize(_rows*_cols)
{
    // computation of rows and columns for padded sub-grids
    localRowsHalo = rows + 2;
    localColsHalo = cols + 2;
    localSizeHalo = localRowsHalo * localColsHalo;

    // allocation for the local grid, local padded grid
    // local padded grid at successive time step
    localState = new int [lifeSize]();
    localObs = new int [localSizeHalo]();
    localObsNext = new int [localSizeHalo]();

    // reading of initial grid
    auto *globalState = new int [lifeSize];
    read_state(filename, globalState, lifeSize);

    std::memcpy(localState, globalState, (lifeSize)*sizeof(int));

    delete [] globalState;

    // initialization of padded grids
    initializeObs();
}

SerialLife::~SerialLife() {
    delete [] localState;
    delete [] localObsNext;
    delete [] localObs;
}

// computation of the alive neighbors
int SerialLife::census(int x, int y) const {
    int dead = localObs[(x - 1)*localColsHalo + (y - 1)] + localObs[(x - 1)*localColsHalo + y] + localObs[(x - 1)*localColsHalo + (y + 1)] +
               localObs[x*localColsHalo + (y - 1)]                                                   + localObs[x*localColsHalo + (y + 1)] +
               localObs[(x + 1)*localColsHalo + (y - 1)] + localObs[(x + 1)*localColsHalo + y] + localObs[(x + 1)*localColsHalo + (y + 1)];
    return (8 - dead);
}

// single step of static evolution
void SerialLife::staticStep() {
    #pragma omp parallel for schedule(static) collapse(2)
        for (int x = 1; x <= rows; x++) {
            for (int y = 1; y <= cols; y++){
                // std::cout << x << " " << y << std::endl;
                int local_population = census(x, y);
                bool lives = (localObs[x*localColsHalo + y] == ALIVE && (local_population == 2 || local_population == 3)) ||
                             (localObs[x*localColsHalo + y] == DEAD && local_population == 3);
                localObsNext[x*localColsHalo + y] = lives ? ALIVE : DEAD;
            }
        }
}

// single step of ordered evolution
void SerialLife::orderedStep() {
    for (int x = 1; x <= rows; x++) {
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

// static evolution
void SerialLife::staticEvolution(int &lifetime, int &record_every) {
    for (int age = 1; age < lifetime; age++) {
        computeHaloRows();
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
void SerialLife::orderedEvolution(int &lifetime, int &record_every) {
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
    freezeGlobalState(lifetime);
}

// system saving through utility functions
void SerialLife::freezeGlobalState(int &age) {
    for (int x = 1; x <= rows; x++) {
        std::memcpy(localState + (x - 1)*cols, localObs + x*localColsHalo + 1, cols*sizeof(int));
    }

    auto *globalState = new int [rows*cols];
    std::memcpy(globalState, localState, (lifeSize)*sizeof(int));
    std::string age_string = std::to_string(age);
    pad_age_string(age_string);
    std::string filename = FNAME_PREFIX + age_string;
    write_state(filename, globalState, rows, cols);

    delete [] globalState;
}

void SerialLife::initializeObs() {
    #pragma omp parallel for schedule(static)
    for (int x = 0; x < rows; x++) {
        for(int y = 0; y < cols; y++) {
            localObs[(x + 1) * localColsHalo + (y + 1)] = localState[x * cols + y];
        }
    }
}

// computation of padded columns
void SerialLife::computeHaloRows() {
    std::memcpy(localObs, localObs + rows*localColsHalo, localColsHalo*sizeof(int));
    std::memcpy(localObs + (localRowsHalo - 1)*localColsHalo, localObs + localColsHalo, localColsHalo*sizeof(int));
}

// computation of padded columns
void SerialLife::computeHaloCols() {
    for (int x = 0; x < localRowsHalo; x++) {
        localObs[x*localColsHalo] = localObs[x*localColsHalo + localColsHalo - 2];
        localObs[x*localColsHalo + localColsHalo - 1] = localObs[x*localColsHalo + 1];
    }
}


