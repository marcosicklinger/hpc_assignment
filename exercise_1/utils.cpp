//
// Created by marcosicklinger on 5/20/23.
//

#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <iostream>
#include "utils.h"

void* generate_random_world(int rows, int cols) {
    int world_size = rows*cols;
    auto* world = new unsigned char[world_size];
    srand((unsigned) time(nullptr));
    for (int i = 0; i < world_size; i++) {
        world[i] = rand()%2 ? (unsigned char) ALIVE : (unsigned char) DEAD;
    }
    return (void*) world;
}

void make_directory() {
    if (!std::filesystem::exists(DIRECTORY)) {
        try {
            std::filesystem::create_directory(DIRECTORY);
        }
        catch (const std::exception& exception) {
            std::cerr << exception.what() << std::endl;
        }
    }
}

void snapshot(const char *filename, void* world_state, int height, int width) {

}

