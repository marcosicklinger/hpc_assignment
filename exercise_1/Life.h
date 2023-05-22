//
// Created by marcosicklinger on 5/21/23.
//

#ifndef LIFE_H
#define LIFE_H

#include <string>

class Life {

public:

    unsigned char *state = nullptr;
    int rows, cols;

    Life(std::string &filename, int &_rows, int &_cols);

    void transition(int &x, int &y);

    void evolve();

private:

};


#endif //LIFE_H
