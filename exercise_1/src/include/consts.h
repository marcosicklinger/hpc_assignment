#ifndef CONSTS_H
#define CONSTS_H

constexpr const char SNAPSHOT[] = "./snapshot/";
constexpr const char TIME[] = "./time/";

constexpr unsigned char ALIVE = 0;
constexpr unsigned char DEAD = 1;

constexpr unsigned int SIZE = 100;

constexpr int ORDERED = 0;

constexpr unsigned int LIFETIME = 100;
constexpr unsigned int RECORD_EVERY = LIFETIME/5;

#endif //CONSTS_H