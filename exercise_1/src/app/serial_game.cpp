#include <iostream>
#include <getopt.h>
#include "../include/utils.h"
#include "../include/consts.h"
#include "../include/SerialLife.h"
#include <chrono>
#include <omp.h>

// default values for initialization and run duration
bool init = false;
bool run = false;
int rows = SIZE;
int cols = SIZE;
bool evolution = ORDERED;
int lifetime = LIFETIME;
int record_every = RECORD_EVERY;
std::string snapshot_filename = DFT_INIT_FNAME;

void print_help(int arg) {
    std::cout << arg << " not found.\n"
                        "Command line arguments options:\n"
                        "-i:        initialization              no argument\n"
                        "-r:        run                         no argument\n"
                        "-h:        height                      positive int\n"
                        "-w:        width                       positive int\n"
                        "-e:        evolution                   int (0: ORDERED, else STATIC)\n"
                        "-f:        source file                 string\n"
                        "-n:        lifetime                    positive int\n"
                        "-s:        snapshot saving rate        positive int\n"
                        "-t:        exe times save path         string\n"
              << std::endl;
    exit(1);
}

void get_args(int argc, char *argv[]) {
    option long_options[] = {
            {"initialization", no_argument, nullptr, 'i'},
            {"run", no_argument, nullptr, 'r'},
            {"height", required_argument, nullptr, 'h'},
            {"width", required_argument, nullptr, 'w'},
            {"evolution", required_argument, nullptr, 'e'},
            {"source file", required_argument, nullptr, 'f'},
            {"lifetime", required_argument, nullptr, 'n'},
            {"snapshot_saving_rate", required_argument, nullptr, 's'},
            {"exec_times_save_path", required_argument, nullptr, 't'},
            {nullptr}
    };
    int arg;
    int opt_idx = 0;
    while ((arg = getopt_long(argc, argv, "irh:w:e:f:n:s:t:", long_options, &opt_idx)) != -1) {
        switch (arg) {
            case 'i':
                init = true;
                break;
            case 'r':
                run = true;
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
                snapshot_filename = optarg;
                break;
            case 'n':
                lifetime = std::stoi(optarg);
                break;
            case 's':
                record_every = std::stoi(optarg);
                break;
            default:
                print_help(arg);
                break;
        }
    }
}

int main(int argc, char *argv[]) {

    get_args(argc, argv);

    if (init) {
        auto *world = generate_random_life(rows, cols);
        write_state(snapshot_filename, world, rows, cols);
        delete [] world;
    }

    if (run) {
        SerialLife life = SerialLife( snapshot_filename, rows, cols);

        double elapsed;
        if (!evolution) {
            auto start = std::chrono::high_resolution_clock::now();
            life.orderedEvolution(lifetime, record_every);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            elapsed = duration.count();
        } else {
            auto start = std::chrono::high_resolution_clock::now();
            life.staticEvolution(lifetime, record_every);
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            elapsed = duration.count();
        }

        #ifdef TSAVE
            int n_threads = omp_get_max_threads();
            std::cout << rows << "\t" << cols << "\t" <<  1 << "\t" <<  n_threads << "\t" << elapsed << std::endl;
        #endif
    }

    return 0;
}
