#ifndef SERIALLIFE_H
#define SERIALLIFE_H


#include <string>

class SerialLife {

    int rows, localRowsHalo;
    int cols, localColsHalo;
    int lifeSize, localSizeHalo;

    int *localState;
    int *localObs;
    int *localObsNext;

    void computeHaloRows();

    void computeHaloCols();

public:

    SerialLife(const std::string &filename, int &_rows, int &_cols);

    ~SerialLife();

    [[nodiscard]] int census(int x, int y) const;

    void staticStep();

    void orderedStep();

    void staticEvolution(int &lifetime, int &record_every);

    void orderedEvolution(int &lifetime, int &record_every);

    void freezeGlobalState(int &age);

    void initializeObs();

    void haloExchange();
};


#endif
