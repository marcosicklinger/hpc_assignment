//
// Created by marcosicklinger on 5/20/23.
//

#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <cstdint>

#define ALIVE 255
#define DEAD 0

#define DIRECTORY "../exercise_1/state"

void* generate_random_world(int rows, int cols);

void make_directory();

void snapshot(int height, int width);

#endif //UTILS_H
