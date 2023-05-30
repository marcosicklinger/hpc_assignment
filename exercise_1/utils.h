//
// Created by marcosicklinger on 5/20/23.
//

#ifndef UTILS_H
#define UTILS_H

unsigned char * generate_random_life(unsigned int &rows, unsigned int &cols);

void make_directory(const std::string &directory);

void write_state(std::string &filename, const char *data, unsigned int &height, unsigned int &width);

unsigned char *read_state_from_pgm(const std::string &filename);

#endif //UTILS_H
