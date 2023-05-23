//
// Created by marcosicklinger on 5/21/23.
//

#ifndef LIFE_H
#define LIFE_H

#include <string>

class Life {

public:

    unsigned char *state = nullptr, *_state = nullptr;
    int rows, cols, size;

    Life(std::string &filename, int &_rows, int &_cols);

    void intraLife(int &_x, int &_y) const;

    void boundaryRowLife(int &_x, int &_y, unsigned char *bordering_row);

    void evolve();

private:

    static int checkPBC(int &x, int &y);

};


#endif //LIFE_H
