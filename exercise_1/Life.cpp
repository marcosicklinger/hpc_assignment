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

void Life::life(int &_x, int &_y) const {
    int x = (_x + rows)%rows;
    int y = (_y + cols)%cols;
    int c = x*cols + y;
    int r_1 = c - cols;
    int r_2 = c + cols;
    int living = 0;
    living += state[c + 1] + state[c - 1];
    for (int i = -1; i < 2; i++) {
        living += state[r_1 + i] + state[r_2 + i];
    }
}

void Life::evolve() {}

int Life::check_pbc(int &x, int &y) {
    return 0;
}



