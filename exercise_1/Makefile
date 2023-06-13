setup:
	mkdir -p snapshot
	mkdir -p time
	mkdir -p simulation

COMPILEFLAGS := -O2 -Wall -fopenmp
SAVINGFLAGS := ${DIRS}

game: src/game.x

src/game.x: src/app/game.cpp src/lib/Life.cpp src/lib/utils.cpp
	mpiCC $(COMPILEFLAGS) $(SAVINGFLAGS) -o $@ $^

ROWS := ${ROWS}
COLS := ${COLS}
S := ${S}
N := ${N}
ordered:
	mpirun -np 1 -x OMP_NUM_THREADS=1 src/game.x -e 0 -i -r -h $(ROWS) -w $(COLS) -f 0 -n $(N) -s $(S) -t ordered.txt

static:

.PHONY: staticweak
staticws:
	@for s in ${SIZE}; do \
		for n in 2; do \
			for _ in $$(seq 10); do \
				mpirun --map-by socket -np $$(cat /proc/cpuinfo | grep "physical id" | sort | uniq | wc -l) -x OMP_PROC_BIND=close -x OMP_NUM_THREADS=$$n src/game.x -e 1 -i -r -h $$s -w $$s -f 0 -n 100 -t staticws; \
	  		done; \
	  	done; \
  	done

.PHONY: staticstrong
staticstrong:

clean: clean_exe clean_snapshot clean_time

clean_exe:
	rm src/*.x

clean_snapshot:
	rm snapshot/*

clean_time:
	rm time/*

clean_sim:
	rm simulation/*