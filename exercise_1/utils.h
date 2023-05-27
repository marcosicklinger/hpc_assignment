//
// Created by marcosicklinger on 5/20/23.
//

#ifndef UTILS_H
#define UTILS_H

constexpr unsigned char ALIVE = 1;
constexpr unsigned char DEAD = 0;

constexpr const char STATE_DIR[] = "../exercise_1/state";

void *generate_random_life(int rows, int cols);

void make_directory(const char *directory);

void write_state(std::string &filename, const void *data, int height, int width);

unsigned char *read_state_from_pgm(std::string &filename);

#endif //UTILS_H
