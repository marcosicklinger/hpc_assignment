setup:
	mkdir -p snapshot
	mkdir -p time
	mkdir -p simulation

COMPILEFLAGS := -O2 -Wall -fopenmp
SAVINGFLAGS := ${DIRS}

game: src/game.x

src/game.x: src/app/game.cpp src/lib/Life.cpp src/lib/utils.cpp
	mpiCC $(COMPILEFLAGS) $(SAVINGFLAGS) -o $@ $^

ROWS ?= 100
COLS ?= 100
S ?= 1
T ?= 100

ordered:
	mpirun -np 1 -x OMP_TUM_THREADS=1 src/game.x -e 0 -i -r -h $(ROWS) -w $(COLS) -f 0 -n $(N) -s $(S) -t ordered.txt

.PHONY: multi_ordered
multi_ordered:
	@for _ in $$(seq 10); do \
		mpirun -np 1 -x OMP_NUM_THREADS=1 src/game.x -e 0 -i -r -h $(ROWS) -w $(COLS) -f 0 -n $(N) -s $(S) -t ordered.txt; \
	done

NP ?= 1
NT ?= 1
static:
	mpirun -np $(NP) -x OMP_NUM_THREADS=$(NT) src/game.x -e 1 -i -r -h $(ROWS) -w $(COLS) -f 0 -n $(N) -s $(S) -t static.txt

SIZE ?= 100
CPS ?= 8
M ?= 1
NS ?= 1

.PHONY: staticomp
staticomp:
	@for s in $(SIZE); do \
		for n in $$(seq $(CPS) ); do \
			for _ in $$(seq $(M)); do \
				mpirun --map-by socket -np $$(cat /proc/cpuinfo | grep "physical id" | sort | uniq | wc -l) -x OMP_PROC_BIND=close -x OMP_NUM_THREADS=$$n src/game.x -e 1 -i -r -h $$s -w $$s -f 0 -n $(T) -t omp.txt; \
	  		done; \
	  	done; \
  	done

.PHONY: staticmpistrong
staticmpistrong:
	@for s in $(SIZE); do \
		for n in $$(seq $$(expr $(CPS) \* $(NS))); do \
			for _ in $$(seq $(M)); do \
				mpirun --map-by core -np $$n -x OMP_PROC_BIND=close -x OMP_NUM_THREADS=1 src/game.x -e 1 -i -r -h $$s -w $$s -f 0 -n $(T) -t mpistrong.txt; \
			done; \
		done; \
	done

.PHONY: statichbstrong
statichbstrong:
	@for s in $(SIZE); do \
		for n in $$(seq $(NS)); do \
			for _ in $$(seq $(M)); do \
				mpirun --map-by core -np $$n -x OMP_PROC_BIND=close -x OMP_NUM_THREADS=$(CPS) src/game.x -e 1 -i -r -h $$s -w $$s -f 0 -n $(T) -t hbstrong.txt; \
			done; \
		done; \
	done

clean: clean_exe clean_snapshot clean_time

clean_exe:
	rm src/*.x

clean_snapshot:
	rm snapshot/*

clean_time:
	rm time/*

clean_sim:
	rm simulation/*