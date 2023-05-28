//
// Created by marcosicklinger on 5/20/23.
//

#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <fstream>
#include "utils.h"

unsigned char * generate_random_life(int rows, int cols) {
    int life_size = rows * cols;
    auto *world = new unsigned char[life_size];
    srand((unsigned) time(nullptr));
    for (int i = 0; i < life_size; i++) {
        world[i] = rand()%2 ? (unsigned char) ALIVE : (unsigned char) DEAD;
    }
    return world;
}

void make_directory(const char *directory) {
    if (!std::filesystem::exists(directory)) {
        try {
            std::filesystem::create_directory(directory);
        }
        catch (const std::exception& exception) {
            std::cerr << exception.what() << std::endl;
        }
    }
}

void write_state(std::string &filename, const char *data, unsigned int height, unsigned int width) {
    make_directory(STATE_DIR);
    std::ofstream state(filename, std::ios_base::out | std::ios::binary | std::ios_base::trunc);
    if (!state) {
        throw std::runtime_error("Error when trying to open the file: " + std::string(filename));
    }
    int max_gray_value{static_cast<int>(ALIVE)};
    state << "P5\n" << width << " " << height << "\n" << max_gray_value << "\n";
    unsigned int state_size = height*width;
    state.write(data, state_size);
    state.close();
}

unsigned char *read_state_from_pgm (const std::string &filename) {
    std::ifstream life_img(filename, std::ios::binary);
    if (!life_img) {
        throw std::runtime_error("Error when trying to retrieve the life_img from file: " + std::string(filename));
    }
    int height, width, max_gray_value;
    std::string format;
    life_img >> format >> width >> height >> max_gray_value;
    auto *state = new unsigned char[height*width];
    life_img.ignore();
    life_img.read(reinterpret_cast<char*>(state), height*width);
    life_img.close();
    return state;
}


