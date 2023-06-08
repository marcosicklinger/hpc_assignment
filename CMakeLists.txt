cmake_minimum_required(VERSION 3.25)
project(hpc_assignment)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(exercise_1)

add_executable(hpc_assignment
        exercise_1/src/utils.cpp
        exercise_1/src/utils.h
        exercise_1/src/game.cpp
        exercise_1/src/Life.h
        exercise_1/src/Life.cpp)
