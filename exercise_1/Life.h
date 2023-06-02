//
// Created by marcosicklinger on 5/21/23.
//

#ifndef LIFE_H
#define LIFE_H

#include <string>

class Life {

private:

    std::string name;
    int n_procs;
    int rank, lrank, urank;
    int lo;
    int hi;
    int rows, localRows, localRowsHalo;
    int cols, localColsHalo;
    int lifeSize, localSize, localSizeHalo;
    unsigned char *localState;
    unsigned char *localObs;
    unsigned char *localObsNext;

    void computeHaloRows();

    void computeHaloCols();

    void computeHaloCorners();

public:

    Life(const std::string &filename, int &_rows, int &_cols);

    ~Life();

    void read_state(std::string &filename);

    int census(unsigned int &x, unsigned int &y) const;

    void staticStep();

    void orderedStep();

    void staticEvolution(int &lifetime, int &record_every);

    void orderedEvolution(int &lifetime, int &record_every);

    unsigned char *getGlobalState();

    void freeze(int &age);

};


#endif //LIFE_H
