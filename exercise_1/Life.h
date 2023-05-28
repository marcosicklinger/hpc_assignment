//
// Created by marcosicklinger on 5/21/23.
//

#ifndef LIFE_H
#define LIFE_H

#include <string>

class Life {

private:

    std::string name;
    unsigned int lo;
    unsigned int hi;
    unsigned int rows, localRows, localRowsHalo;
    unsigned int cols, localColsHalo;
    unsigned int lifeSize;
    unsigned char *localState;
    unsigned char *localObs;
    unsigned char *localObsNext;

    void computeHaloRows(); // to be computed for ordered evolution only

    void computeHaloCols();

    void computeHaloCorners();

public:

    Life(const std::string &filename, int &_rows, int &_cols);

    ~Life();

    void read_state(std::string &filename);

    int census(unsigned int &x, unsigned int &y) const;

    void staticStep();

    void orderedStep();

    void evolution(int &time, int record_every);

    unsigned char *getGlobalState();

    void freeze(int &age);

};


#endif //LIFE_H
