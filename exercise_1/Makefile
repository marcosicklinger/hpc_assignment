CXXFLAGS := -O2 -Wall -fopenmp

setup:
	mkdir -p snapshot
	mkdir -p time

static: src/static.x

src/static.x: src/app/game.cpp src/lib/Life.cpp src/lib/utils.cpp
	mpiCC $(CXXFLAGS) -o $@ $^

clean: clean_exe clean_snapshot clean_time

clean_exe:
	rm src/*.x

clean_snapshot:
	rm snapshot/*

clean_time:
	rm time/*