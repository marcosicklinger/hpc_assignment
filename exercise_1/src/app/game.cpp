#include <iostream>
#include <fstream>
#include <getopt.h>
#include <filesystem>
#include "../include/utils.h"
#include "../include/consts.h"
#include "../include/Life.h"
#include <mpi.h>
#include <omp.h>

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

    MPI_Init(&argc, &argv);

    int rank, n_procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);

    get_args(argc, argv);

    if (init && rank == 0) {
        auto *world = generate_random_life(rows, cols);
        write_state(snapshot_filename, world, rows, cols);
        delete [] world;
    }

    if (run) {
        if (rank == 0) {
            try {
                if (!std::filesystem::exists(snapshot_filename + ".pgm")) {
                    throw std::runtime_error("Error when trying to read the file: " + snapshot_filename);
                }
            } catch (const std::exception& exception) {
                std::cerr << exception.what() << std::endl;
            }
        }

        Life life = Life( snapshot_filename, rows, cols, n_procs, rank);

        double this_elapsed;
        if (!evolution) {
            this_elapsed = -MPI_Wtime();
            life.orderedEvolution(lifetime, record_every);
            this_elapsed += MPI_Wtime();
        } else {
            this_elapsed = -MPI_Wtime();
            life.staticEvolution(lifetime, record_every);
            this_elapsed += MPI_Wtime();
        }

        #ifdef TSAVE
            double elapsed_avg;
            MPI_Reduce(&this_elapsed, &elapsed_avg, n_procs,
                       MPI_DOUBLE, MPI_SUM,
                       0,
                       MPI_COMM_WORLD);
            elapsed_avg /= n_procs;

            if (rank == 0) {
                int n_threads = omp_get_max_threads();
                std::cout << rows << "\t" << cols << "\t" <<  n_procs << "\t" <<  n_threads << "\t" << elapsed_avg << std::endl;
            }
        #endif
    }

    MPI_Finalize();

    return 0;
}