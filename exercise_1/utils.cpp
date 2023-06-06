//
// Created by marcosicklinger on 5/20/23.
//

#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <fstream>
#include "utils.h"
#include "consts.h"

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

void read_state_from_pgm (unsigned char *dest, const std::string &filename) {
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
