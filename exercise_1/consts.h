//
// Created by marcosicklinger on 5/29/23.
//

#ifndef CONSTS_H
#define CONSTS_H

constexpr const char STATE_DIR[] = "../exercise_1/state";

constexpr unsigned char ALIVE = 1;
constexpr unsigned char DEAD = 0;

constexpr bool INIT = true;
constexpr bool RUN = false;

constexpr unsigned int SIZE = 100;

constexpr int ORDERED = 0;

constexpr unsigned int LIFETIME = 100;
constexpr unsigned int RECORD_EVERY = LIFETIME/5;

const std::string FILENAME = "life";

#endif //CONSTS_H
