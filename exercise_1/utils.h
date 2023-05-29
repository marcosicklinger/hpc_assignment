//
// Created by marcosicklinger on 5/20/23.
//

#ifndef UTILS_H
#define UTILS_H

constexpr const char STATE_DIR[] = "../exercise_1/state";

unsigned char * generate_random_life(int rows, int cols);

void make_directory(const char *directory);

void write_state(std::string &filename, const char *data, unsigned int height, unsigned int width);

unsigned char *read_state_from_pgm(const std::string &filename);

#endif //UTILS_H
