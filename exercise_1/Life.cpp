//
// Created by marcosicklinger on 5/21/23.
//

#include "Life.h"
#include "utils.h"

Life::Life(std::string &filename, int &_rows, int &_cols): rows(_rows), cols(_cols), size(_rows*_cols) {
    state = read_state(filename);
}

void Life::transition(int &w) const {

}

void Life::evolve() {}

int Life::check_pbc(int &w) {
    return 0;
}



