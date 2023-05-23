//
// Created by marcosicklinger on 5/20/23.
//

#include <iostream>
#include "utils.h"

int main() {
    std::string filename = static_cast<std::string>(STATE_DIR) + "/" + "trial_0.pgm";
    auto *world = generate_random_life(300, 300);
    for (int i = 0; i < 9; i++) {
        std::cout << static_cast<int>(static_cast<unsigned char *>(world)[i]) << " ";
    }
    std::cout << std::endl;
    write_state(filename, world, 300, 300);

    auto *uploaded_world = read_state(filename);
    for (int i = 0; i < 9; i++) {
        std::cout << static_cast<int>(uploaded_world[i]) << " ";
    }
    std::cout << std::endl;

    delete[] uploaded_world;

    std::cout << -1%10<< std::endl;

    return 0;
}