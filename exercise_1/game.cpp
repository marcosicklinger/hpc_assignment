//
// Created by marcosicklinger on 5/20/23.
//

#include <iostream>
#include "utils.h"

int main() {
    auto* world = static_cast<unsigned char *>(generate_random_world(3, 3));
    for (int i = 0; i < 9; i++) {
        std::cout << static_cast<int>(world[i]) << " ";
    }
    std::cout << std::endl;

    make_directory();
}