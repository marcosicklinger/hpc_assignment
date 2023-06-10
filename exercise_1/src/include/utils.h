#ifndef UTILS_H
#define UTILS_H

unsigned char * generate_random_life(int &rows, int &cols);

void make_directory(const std::string &directory);

void write_state(std::string &filename, void *data, int &height, int &width);

void read_state_from_pgm(unsigned char *dest, const std::string &filename);

void write_time(std::string &filename, int n, double time);

double mean(const double *values, int size);

#endif //UTILS_H
