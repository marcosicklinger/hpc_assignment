### Structure

The second exercise directory possesses the following structure:
```angular2html
├── analysis
│   ├── cores.py
│   └── sizes.py
├── data
│   ├── core_scaling
│   └── size_scaling
├── gemm.c
├── gemm.cpp
├── Makefile
├── cores.sh
└── sizes.sh
``` 
Five files, one makefile, two bash scripts (see explanation below) and two C/C++ files, are located right in the first level of the directory.\
The `analysis` directory contains python scripts for graphical analysis of the data.

The data are contained in the `data` directory, with `core_scaling` and `size_scaling` sub-directories. Data are named as:
- `cc`: places: cores, policy: close
- `cs`: places: cores, policy: spread
- `sc`: places: sockets, policy: close
- `ss`: places: sockets, policy: spread
- `tc`: places: cores, policy: close
- `ts`: places: threads, policy: spread

### Specifics

#### Framework

**_Makefile_** \
The make file contains a few applications useful to set upe the directory:
- `setup`: running `make setup` will create a tree of directories (inside the exercise main directory) needed for storing useful time measurements for both single and double precision.
    ```angular2html
    ├── cores
    │   ├── double
    │   └── single
    └── sizes
        ├── double
        └── single
    ```
  `sizes` directory is used size scaling simulations, while `cores` for core scaling simulations.
- `executable`: running `make executable` will grant executable permission to the scripts `sizes.sh` and `cores.sh`.
- `all`: application for the program compilation.
- Other applications are `clean_exe`, `clean_sizes`, `clean_cores`.

### How to run

#### Compiling the program
Using the command `srun -n 1 make all PREC=precision` (in the `exercise_2` directory) will create the executables in the current directory. The value `precision` can be either `"-DUSE_SINGLE"` or `"-DUSE_FLOAT"`. It makes use of the applications `oblas.x` and `mkl.x` in the makefile, which directly compiles the code with the needed libraries and flags.

#### Running the program
In order to run the program, use `srun -n 1 ./executable_name K1 K2 K3` (in the `exercise_2` directory), where:

- `executable_name` should be either `oblas.x` or `mkl.x`, according to the compilation step described above;
- `K*` are the matrices linear sizes.

#### Implementation details
The following module was exploited for the runs:
- Architecture: `architecture/AMD`;
- Math libraries: `openBLAS/0.3.2023`, `mkl/2022.2.1`.
