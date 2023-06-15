#ifndef UTILS_H
#define UTILS_H

int * generate_random_life(int &rows, int &cols);

void write_state(std::string &filename, int *data, int &height, int &width);

void read_pgm_file(const std::string &filename, unsigned char *dest);

void read_state(const std::string &filename, int *state, int size);

void write_time(std::string &filename, int rows, int cols, int n_threads, int n_procs, double time);

#endif //UTILS_H
