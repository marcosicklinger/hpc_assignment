CXXFLAGS := -O2 -Wall -fopenmp

setup:
	mkdir -p snapshot
	mkdir -p time

static: src/static.x

src/static.x: src/game.cpp src/Life.cpp src/utils.cpp
	mpiCC $(CXXFLAGS) -o $@ $^

