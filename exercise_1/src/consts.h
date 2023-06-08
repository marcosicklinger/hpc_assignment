//
// Created by marcosicklinger on 5/29/23.
//

#ifndef CONSTS_H
#define CONSTS_H

constexpr const char STATE_DIR[] = "./state";
constexpr const char TMEASURE_DIR[] = "./time";
constexpr const char FILENAME[] = "life";
constexpr const char TMEASURE_FILENAME[] = "tmeasure.txt";

constexpr unsigned char ALIVE = 0;
constexpr unsigned char DEAD = 1;

constexpr bool INIT = true;
constexpr bool RUN = false;

constexpr unsigned int SIZE = 100;

constexpr int ORDERED = 0;

constexpr unsigned int LIFETIME = 100;
constexpr unsigned int RECORD_EVERY = LIFETIME/5;

#endif //CONSTS_H
