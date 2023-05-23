//
// Created by marcosicklinger on 5/21/23.
//

#include <cstring>
#include "Life.h"
#include "utils.h"

Life::Life(std::string &filename, int &_rows, int &_cols): rows(_rows), cols(_cols), size(_rows*_cols) {
    state = read_state(filename);
    _state = new unsigned char[size];
    size_t data_size = size*sizeof(unsigned char);
    memcpy(_state, state, data_size);
}

void Life::intraLife(int &_x, int &_y) const {
    int c = _x*cols + _y;
    int r_1 = c - cols;
    int r_2 = c + cols;
    int living = state[r_1 - 1] + state[r_1] + state[r_1 + 1] +
                 state[c - 1]                    + state[c + 1] +
                 state[r_2 - 1] + state[r_2] + state[r_2 + 1];

}

void Life::boundaryRowLife(int &_x, int &_y, unsigned char *bordering_row) {
    int living = 0;
}

void Life::evolve() {}

int Life::checkPBC(int &x, int &y) {
    return 0;
}




