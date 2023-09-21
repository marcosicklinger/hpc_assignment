cmake_minimum_required(VERSION 3.25)
project(hpc_assignment)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(exercise_1
                    exercise_2)

add_executable(hpc_assignment
        exercise_1/src/lib/utils.cpp
        exercise_1/src/include/utils.h
        exercise_1/src/app/game.cpp
        exercise_1/src/include/Life.h
        exercise_1/src/lib/Life.cpp
        exercise_2/gemm.cpp
        exercise_2/gemm.c
        exercise_1/src/lib/SerialLife.cpp
        exercise_1/src/include/SerialLife.h
        exercise_1/src/app/serial_game.cpp
)
