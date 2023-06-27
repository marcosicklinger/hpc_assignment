#ifndef UTILS_H
#define UTILS_H

int * generate_random_life(int &rows, int &cols);

void write_state(std::string &filename, const int *data, int &height, int &width);

void read_pgm_file(const std::string &filename, unsigned char *dest);

void read_state(const std::string &filename, int *state, int size);

void write_time(int rows, int cols, int n_threads, int n_procs, double time);

void pad_age_string(std::string &age_string);

#endif //UTILS_H
