//
// Created by marcosicklinger on 5/20/23.
//

#include <iostream>
#include "utils.h"

int main() {
    int a{};
    std::string filename = static_cast<std::string>(STATE_DIR) + "/" + "trial_0.pgm";
    int k = 30;
    auto *world = generate_random_life(k, k);
    for (int i = 0; i < k; i++) {
        std::cout << static_cast<int>(static_cast<unsigned char *>(world)[i]) << " ";
    }
    std::cout << std::endl;
    write_state(filename, reinterpret_cast<const char*>(world), k, k);

    auto *uploaded_world = read_state_from_pgm(filename);
    for (int i = 0; i < k; i++) {
        std::cout << static_cast<int>(uploaded_world[i]) << " ";
    }
    std::cout << std::endl;

    delete[] uploaded_world;

    std::cout << -1%10 << ' ' << a << std::endl;

    return 0;
}