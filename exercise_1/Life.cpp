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

void Life::transition(int &c) const {
    int r_1 = c - cols;
    int r_2 = c + cols;

}

void Life::evolve() {}

int Life::check_pbc(int &w) {
    return 0;
}



