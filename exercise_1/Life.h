//
// Created by marcosicklinger on 5/21/23.
//

#ifndef LIFE_H
#define LIFE_H

#include <string>

class Life {

public:

    unsigned char *state = nullptr;
    int rows, cols, size;

    Life(std::string &filename, int &_rows, int &_cols);

    void transition(int &w) const;

    void evolve();

private:

    static int check_pbc(int &w);

};


#endif //LIFE_H
