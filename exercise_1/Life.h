//
// Created by marcosicklinger on 5/21/23.
//

#ifndef LIFE_H
#define LIFE_H

#include <string>

class Life {

private:

    unsigned int lo;
    unsigned int hi;
    unsigned int rows, localRows, extendedLocalRows;
    unsigned int cols, extendedLocalCols;
    size_t lifeSize;
    unsigned char *globalState;
    unsigned char *localState;
    unsigned char *localObservation;
    unsigned char *extendedLocalObservation;

    void computeHaloRows(); // to be computed for ordered evolution only

    void computeHaloCols();

    void computeHaloCorners();

public:

    Life(std::string &filename, int &_rows, int &_cols);

    ~Life();

    void read_state(std::string &filename);

    int living(int &_x, int &_y) const;

    void evolve();

};


#endif //LIFE_H
