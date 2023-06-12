#ifndef UTILS_H
#define UTILS_H

unsigned char * generate_random_life(int &rows, int &cols);

void make_directory(const std::string &directory);

void write_state(std::string &filename, void *data, int &height, int &width);

void read_pgm_file(const std::string &filename, unsigned char *dest);

void read_state(const std::string &filename, int *state, int size);

void write_time(std::string &filename, int rows, int cols, int n, double time);

double mean(const double *values, int size);

#endif //UTILS_H
