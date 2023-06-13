#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <fstream>
#include "../include/utils.h"
#include "../include/consts.h"

int * generate_random_life(int &rows, int &cols) {
    int life_size = rows * cols;
    auto *world = new int[life_size];
    srand((unsigned) time(nullptr));
    for (int i = 0; i < life_size; i++) {
        world[i] = rand()%2 ? ALIVE : DEAD;
    }
    return world;
}

void write_state(std::string &filename, int *data, int &height, int &width) {
    std::ofstream state(filename, std::ios_base::out | std::ios::binary | std::ios_base::trunc);
    try {
        if (!state) {
            throw std::runtime_error("Error when trying to open the file: " + std::string(filename));
        }
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << std::endl;
    }
    int max_gray_value{DEAD};
    state << "P5\n" << width << " " << height << "\n" << max_gray_value << "\n";
    int state_size = height*width;
    auto *writable_data = new unsigned char [state_size];
    for (int n = 0; n < state_size; n++) {
        writable_data[n] = static_cast<unsigned char>(data[n]);
    }
    state.write(reinterpret_cast<const char*>(writable_data), state_size);
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
