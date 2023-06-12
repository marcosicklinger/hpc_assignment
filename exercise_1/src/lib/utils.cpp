#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <fstream>
#include "../include/utils.h"
#include "../include/consts.h"

unsigned char * generate_random_life(int &rows, int &cols) {
    int life_size = rows * cols;
    auto *world = new unsigned char[life_size];
    srand((unsigned) time(nullptr));
    for (int i = 0; i < life_size; i++) {
        world[i] = rand()%2 ? (unsigned char) ALIVE : (unsigned char) DEAD;
    }
    return world;
}

void make_directory(const std::string &directory) {
    if (!std::filesystem::exists(directory)) {
        try {
            std::filesystem::create_directory(directory);
        }
        catch (const std::exception& exception) {
            std::cerr << exception.what() << std::endl;
        }
    }
}

void write_state(std::string &filename, void *data, int &height, int &width) {
    std::ofstream state(filename, std::ios_base::out | std::ios::binary | std::ios_base::trunc);
    try {
        if (!state) {
            throw std::runtime_error("Error when trying to open the file: " + std::string(filename));
        }
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << std::endl;
    }
    int max_gray_value{static_cast<int>(DEAD)};
    state << "P5\n" << width << " " << height << "\n" << max_gray_value << "\n";
    unsigned int state_size = height*width;
    state.write(reinterpret_cast<const char*>(data), state_size);
    state.close();
}

void read_pgm_file (const std::string &filename, unsigned char *dest) {
    std::ifstream life_img(filename, std::ios::binary);
    int height, width, max_gray_value;
    std::string format;
    life_img >> format >> width >> height >> max_gray_value;
    life_img.ignore();
    life_img.read(reinterpret_cast<char*>(dest), height*width);
    life_img.close();
}


double mean(const double *values, int size) {
    if (size == 0) {
        return 0;
    }

    double sum = 0.0, norm = 1./size;

    #pragma omp parallel for reduction(+:sum) schedule(static)
    for (int n = 0; n < size; n++) {
        sum += values[n];
    }
    sum *= norm;

    return sum;
}

void write_time(std::string &filename, int rows, int cols, int n, double time){
    std::ofstream ofile;
    if (!std::filesystem::exists(filename)) {
        ofile.open(filename, std::ios_base::out);
        ofile << "h" << "\t" << "w" << "\t" << "n" << "\t" << "time" << std::endl;
    } else {
        ofile.open(filename,std::ios_base::out | std::ios::app);
    }
    ofile << rows << "\t" << cols << "\t" <<  n << "\t" << time << std::endl;
    ofile.close();
}
void read_state(const std::string &filename, int *state, int size){
    auto *file_pgm = new unsigned char[size];
    read_pgm_file(filename, file_pgm);
    for (int n = 0; n < size; n++) {
        state[n] = static_cast<int>(file_pgm[n]);
    }
}
