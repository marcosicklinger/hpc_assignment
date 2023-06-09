CXXFLAGS := -O2 -Wall -fopenmp

setup:
	mkdir -p snapshot
	mkdir -p time

static: static.x

static.x: src/game.cpp src/Life.cpp src/utils.cpp
	mpiCC $(CXXFLAGS) -o $@ $^

