//
// Created by marcosicklinger on 5/20/23.
//

#include <iostream>
#include <getopt.h>
#include "utils.h"
#include "consts.h"

bool init = INIT;
unsigned int rows = SIZE;
unsigned int cols = SIZE;
bool evolution = ORDERED;
unsigned int lifetime = LIFETIME;
unsigned int record_every = RECORD_EVERY;
std::string filename = FILENAME;

void printHelp() {
    std::cout <<
                "-n:        initialization - no argument\n"
                "-r:        run - no argument\n"
                "-h:        height - int (>=100)\n"
                "-w:        width - int (>=100)\n"
                "-e:        evolution\n"
                "-f:        source file\n"
                "-n:        lifetime\n"
                "-s:        record rate\n"
                "-help:     help"
    << std::endl;
    exit(1);
}

int main(int argc, char *argv[]) {
    option long_options[] = {
            {"initialization", no_argument, nullptr, 'i'},
            {"run", no_argument, nullptr, 'r'},
            {"height", required_argument, nullptr, 'h'},
            {"width", required_argument, nullptr, 'w'},
            {"evolution", required_argument, nullptr, 'e'},
            {"source file", required_argument, nullptr, 'f'},
            {"lifetime", required_argument, nullptr, 'n'},
            {"record rate", required_argument, nullptr, 's'},
            {nullptr}
    };
    int arg;
    int opt_idx = 0;
    while ((arg = getopt_long(argc, argv, "irh:w:e:f:n:s:", long_options, &opt_idx)) != -1) {
        switch (arg) {
            case 'i':
                break;
            case 'r':
                init = RUN;
                break;
            case 'h':
                rows = std::stoi(optarg);
                break;
            case 'w':
                cols = std::stoi(optarg);
                break;
            case 'e':
                evolution = std::stoi(optarg);
                break;
            case 'f':
                filename = optarg;
                break;
            case 'n':
                lifetime = std::stoi(optarg);
                break;
            case 's':
                record_every = std::stoi(optarg);
                break;
            case 'help':
            default:
                printHelp();
                break;
        }
    }

    int a{};
    filename = static_cast<std::string>(STATE_DIR) + filename;
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