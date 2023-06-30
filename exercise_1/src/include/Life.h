#ifndef LIFE_H
#define LIFE_H

#include <string>

class Life {

private:

    int nTasks;
    int rank;
    int loRank, upRank;

    int rows, localRows, localRowsHalo;
    int cols, localColsHalo;
    int lifeSize, localSize, localSizeHalo;

    int *localState;
    int *localObs;
    int *localObsNext;

    void computeHaloRows();

    void computeHaloCols();

    public:

    Life(const std::string &filename, int &_rows, int &_cols, int np, int rk);

    ~Life();

    void staticStep();

    void orderedStep();

    void staticEvolution(int &lifetime, int &record_every);

    void orderedEvolution(int &lifetime, int &record_every);

    void freezeGlobalState(int &age);

    void initializeObs();

    void haloExchange();
};


#endif //LIFE_H
