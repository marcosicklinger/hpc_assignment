//
// Created by marcosicklinger on 5/20/23.
//

#ifndef UTILS_H
#define UTILS_H

unsigned char * generate_random_life(int &rows, int &cols);

void make_directory(const std::string &directory);

void write_state(std::string &filename, const char *data, int &height, int &width);

void read_state_from_pgm(unsigned char *dest, const std::string &filename);

#endif //UTILS_H
