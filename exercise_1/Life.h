//
// Created by marcosicklinger on 5/21/23.
//

#ifndef LIFE_H
#define LIFE_H

#include <string>

class Life {

private:

    std::string loc;
    int lo, hi;
    int loRow, hiRow;
    int loRank, upRank;
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

    Life(std::string location, std::string filename, int &_rows, int &_cols);

    ~Life();

    void read_state(std::string &filename);

    [[nodiscard]] unsigned char census(int x, int y) const;

    void staticStep();

    void orderedStep();

    void staticEvolution(int &lifetime, int &record_every);

    void orderedEvolution(int &lifetime, int &record_every);

    unsigned char *getGlobalState();

    void freeze(int &age);

    [[nodiscard]] int getHi() const { return lo; }

    [[nodiscard]] int getLo() const{ return lo; };

    [[nodiscard]] int getLocalSize() const{ return localSize; };

    [[nodiscard]] int getLifeSize() const{ return lifeSize; };

    unsigned char &operator[](int globalIdx);

    void initialize(void* data);

    void initializeObs();

    void haloExchange();
};


#endif //LIFE_H
