//
// Created by marcosicklinger on 5/20/23.
//

#include <iostream>
#include <getopt.h>
#include <filesystem>
#include "utils.h"
#include "consts.h"
#include "Life.h"
#include <mpi.h>
#include <omp.h>

bool init = INIT;
int rows = SIZE;
int cols = SIZE;
bool evolution = ORDERED;
int lifetime = LIFETIME;
int record_every = RECORD_EVERY;
std::string filename = FILENAME;

void print_help() {
    std::cout <<
                "Command line arguments options:\n"
                "-i:        initialization      no argument\n"
                "-r:        run                 no argument\n"
                "-h:        height              unsigned int\n"
                "-w:        width               unsigned int\n"
                "-e:        evolution           int (0: ORDERED, else STATIC)\n"
                "-f:        source file         string\n"
                "-n:        lifetime            unsigned int\n"
                "-s:        record rate         unsigned int\n"
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
            default:
                print_help();
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, n_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

    get_args(argc, argv);

    std::string directory = static_cast<std::string>(STATE_DIR) + "/plife" +
                            std::to_string(rows) + "x" + std::to_string(cols) + "/";

    if (init && rank == 0) {
        std::string instance_name = directory + "0";
        make_directory(directory);
        auto *world = generate_random_life(rows, cols);
        write_state(instance_name, world, rows, cols);

        delete [] world;
    } else if (!init) {
        try {
            if (!std::filesystem::exists(directory + filename)) {
                throw std::runtime_error("Error when trying to open the file: " + std::string(filename));
            }
        } catch (const std::exception& exception) {
            std::cerr << exception.what() << std::endl;
        }

        Life life = Life(directory, filename, rows, cols);

        omp_set_num_threads(4);
        if (!evolution) {
            life.orderedEvolution(lifetime, record_every);
        } else {
            life.staticEvolution(lifetime, record_every);
        }

//        double *all_elapsed = nullptr;
//        if (rank == 0) {
//            all_elapsed = new double [n_procs];
//        }
        double this_elapsed = life.getElapsed();
//        MPI_Gather(&this_elapsed, 1, MPI_DOUBLE,
//                   all_elapsed + rank, 1, MPI_DOUBLE,
//                   0, MPI_COMM_WORLD);
//        if (rank == 0) {
//            for (int i = 0; i < n_procs; i++) {
//                std::cout << all_elapsed[i] << std::endl;
//            }
//            std::cout << "\t" << mean(all_elapsed, n_procs) << std::endl;
//        }
//        delete [] all_elapsed;

        double elapsed_avg;
        MPI_Reduce(&this_elapsed, &elapsed_avg, n_procs,
                   MPI_DOUBLE, MPI_SUM,
                   0,
                   MPI_COMM_WORLD);
        elapsed_avg /= n_procs;
        if (rank == 0) {
            std::cout << "\t" << elapsed_avg << std::endl;
        }
    }

    MPI_Finalize();

    return 0;
}