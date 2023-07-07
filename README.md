# Foundations of High Performance Computing: final assignment
## Exercise 1: Parallel Game of Life
The objective of this exercise is to implement a parallel version of Conway's Game of Life using a hybrid OpenMP-MPI approach. Further details on Conway's Game of Life can be found [here](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life).

<p align="center">
  <img src="./exercise_1/display/life.gif" alt="Alt Text" width="862" height="256">
</p>

### Structure
The structure of the first exercise's directory is the following:
```angular2html
├── Makefile
├── mpiscal.sh
├── ompscal.sh
├── ordered.sh
└── src
    ├── app
    │   ├── game.cpp
    │   ├── graph.py
    │   ├── life.py
    │   └── story.py
    ├── include
    │   ├── consts.h
    │   ├── Life.h
    │   └── utils.h
    └── lib
        ├── Life.cpp
        └── utils.cpp
```
Four files, one makefile and three bash scripts (see explanation below), are located right in the first level of the directory.\
The `src` subdirectory contains the core of the application:
- `include`: contains the needed header files. These are `consts.h`, in which constants whose value is known at compile time are defined, `utils.h`, containing utility methods, and `Life.h`, where the class representing the Game of Life is declared;
- `lib`: stores the implementation of the utility methods declared in `utils.h`, and the implementation of the `Life` class methods;
- `app`: contains the `main` and additional python files for plotting; `graph.py` is needed for plotting relevant graphs regarding the performance analysis, while `life.py` and `story.py` are utilized to plot a life snapshot and simulating an entire life story, respectively.

### Specifics

#### Framework

**_Makefile_** \
The make file contains a few applications useful to set upe the directory:
- `setup`: running `make setup` will create a few useful directories needed for storing useful data. 
  - `exe`: directory containing executables; 
  - `time`: should contain time measurements;
  - `snapshot`: should contain life's snapshots that are saved during a run;
  - `simulation`: can be used as directory to save life stories.
- `executable`: running `make executable` will grant executable permission to the scripts `ordered.sh`, `ompscal.sh` and `mpiscal.sh`.
- `game`: running `make game SAVINGFLAGS=opt` will create a game executable in `src/exe`. It makes use of the application `src/exe/game.x` which directly compiles the code with the needed libraries and flags; `opt` takes the following values:
  - `"-DTSAVE""`: directive for saving time measurements;
  - `"-DSSAVE""`: directive for saving life's snapshots.
- Other applications are `clean_exe`, `clean_time`, `clean_snapshot`, `clean_sim` are needed for cleaning subdirectories; `clean` uses all of them but `clean_sim`. The `simulation` directory is meant for processing data on local hosts, so the user is free to save as many life stories as needed.

**_Scripts_** 
- `ordered.sh`: runs ordered evolution;
- `ompscal.sh`: runs OpenMP scalability simulations;
- `mpiscal.sh`: runs OpenMPI scalability simulations.

#### Implementation details
The following module was exploited for the runs:
- Architecture: `architecture/AMD`;
- Message Passing Interface: `openMPI/4.1.5/gnu/12.2.1`;

## Exercise 2: OpenBLAS and MKL for matrix-matrix multiplication
The purpose of this task is to compare the performance of OpenBLAS and MKL libraries applied matrix-matrix multiplication task.

### Structure

The second exercise directory possesses the following structure:
```angular2html
├── analysis
│   ├── strong.py
│   └── weak.py
├── gemm.c
├── gemm.cpp
├── Makefile
├── strong.sh
└── weak.sh
``` 
Four files, one makefile, 2 bash scripts (see explanation below) and two C/C++ files, are located right in the first level of the directory.\
The `analysis` directory contains python scripts for graphical analysis of the data.

### Specifics

#### Framework

**_Makefile_** \
The make file contains a few applications useful to set upe the directory:
- `setup`: running `make setup` will create a tree of directories (inside the exercise main directory) needed for storing useful time measurements for both single and double precision. 
    ```angular2html
    ├── strong
    │   ├── double
    │   └── single
    └── weak
        ├── double
        └── single
    ```
  `weak` directory is used size scaling simulations, while `strong` for core scaling simulations.
- `executable`: running `make executable` will grant executable permission to the scripts `weak.sh` and `strong.sh`.
- `all`: running `make all` will create the executables in the current directory. It makes use of the applications `oblas.x` and `mkl.x` which directly compiles the code with the needed libraries and flags.
- Other applications are `clean_exe`, `clean_weak`, `clean_strong`.

**_Scripts_** 
- `weak.sh`: runs size scaling analysis;
- `strong.sh`: runs core scaling analysis;

#### Implementation details
The following module was exploited for the runs:
- Architecture: `architecture/AMD`;
- Math libraries: `openBLAS/0.3.2023`, `mkl/2022.2.1`.
