#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include "../include/utils.h"
#include "../include/consts.h"

// generate random life pattern
int * generate_random_life(int &rows, int &cols) {
    int life_size = rows * cols;
    auto *world = new int[life_size];
    srand((unsigned) time(nullptr));
    for (int i = 0; i < life_size; i++) {
        world[i] = rand()%2 ? ALIVE : DEAD;
    }
    return world;
}

// write state to file
void write_state(std::string &filename, const int *data, int &height, int &width) {
    // create and check output stream instance
    std::ofstream state(filename + ".pgm", std::ios_base::out | std::ios::binary | std::ios_base::trunc);
    try {
        if (!state) {
            throw std::runtime_error("Error when trying to write the file: " + std::string(filename));
        }
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << std::endl;
    }

    unsigned char max_pixel_value = 255;
    int max_gray_value = static_cast<int>(max_pixel_value);
    // create header in file
    state << "P5\n" << width << " " << height << "\n" << max_gray_value << "\n";

    int state_size = height*width;
    auto *writable_data = new unsigned char [state_size];
    for (int n = 0; n < state_size; n++) {
        // cast data to char
        writable_data[n] = static_cast<unsigned char>(data[n])*max_pixel_value;
    }

    // write data to file
    state.write(reinterpret_cast<const char*>(writable_data), state_size);
    delete [] writable_data;
    state.close();
}

// read data from pgm file
void read_pgm_file (const std::string &filename, unsigned char *dest) {
    // create and check input stream instance
    std::ifstream life_img(filename + ".pgm", std::ios::binary);
    try {
        if (!life_img) {
            throw std::runtime_error("Error when trying to read the file: " + std::string(filename));
        }
    } catch (const std::exception& exception) {
        std::cerr << exception.what() << std::endl;
    }
    int height, width, max_gray_value;
    std::string format;
    life_img >> format >> width >> height >> max_gray_value;
    life_img.ignore();
    life_img.read(reinterpret_cast<char*>(dest), height*width);
    life_img.close();
}

// read data and store it to a buffer
void read_state(const std::string &filename, int *state, int size){
    auto *file_pgm = new unsigned char[size];
    read_pgm_file(filename, file_pgm);
    for (int n = 0; n < size; n++) {
        state[n] = static_cast<int>(file_pgm[n])/255;
    }
    delete [] file_pgm;
}

// pad age (evolution iteration count) to format 0000x
void pad_age_string(std::string &age_string) {
    int num_digs_in_age = static_cast<int>(age_string.length());
    if (num_digs_in_age < 5) {
        age_string = std::string(5 - num_digs_in_age, '0') + age_string;
    }
}


