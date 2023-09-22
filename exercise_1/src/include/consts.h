#ifndef CONSTS_H
#define CONSTS_H

// prefix for snapshot filename
constexpr const char FNAME_PREFIX[] = "./snapshot/snapshot_";

// default name for first snapshot
constexpr const char DFT_INIT_FNAME[] = "./snapshot/snapshot_00000";

// cells' values
constexpr int ALIVE = 0;
constexpr int DEAD = 1;

// default size
constexpr unsigned int SIZE = 100;

// default is ordered evolution
constexpr int ORDERED = 0;

// default number of time iterations
constexpr unsigned int LIFETIME = 100;

// default value for snapshot saving frequency
constexpr unsigned int RECORD_EVERY = 20;

#endif //CONSTS_H
