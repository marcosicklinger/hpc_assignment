# LOOP VARIABLES
NTRIALS := 1
NTASKS := 1
NTHREADS := 1
NSOCKS := 1

# LIFE SIZE, RECORDING RATE, LIFETIME
SIZE := 100
S := 1
T := 100

# STARTING SNAPSHOT NAME
FNAME := 0
TFNAME := time.txt

CPS := 8

# COMPILATION FLAGS
COMPILEFLAGS := -O2 -Wall -Wstrict-aliasing -fopenmp
SAVINGFLAGS := ${DIRS}

# RUNNING FLAGS
MAPFLAG := --map-by socket
ORDEREDFLAGS := -e 0 -i -r
STATICFLAGS := -e 1 -i -r

# SETUP OF DIRECTORY
setup:
	mkdir -p snapshot
	mkdir -p time
	mkdir -p simulation

# COMPILE APPLICATION
game: src/game.x

src/game.x: src/app/game.cpp src/lib/Life.cpp src/lib/utils.cpp
	mpiCC $(COMPILEFLAGS) $(SAVINGFLAGS) -o $@ $^

# ORDERED RUN
ordered:
	@for s in $(SIZE); do \
		for _ in $$(seq $(NTRIALS)); do \
			mpirun -np 1 -x OMP_NUM_THREADS=1 src/game.x $(ORDEREDFLAGS) -h $$s -w $$s -f $(FNAME) -n $(T) -s $(S) -t $(TFNAME); \
		done; \
	done

# STATIC RUN
static:
	@for s in $(SIZE); do \
  		for n in $(NTASKS); do \
  			for m in $(NTHREADS); do \
		  		for _ in $$(seq $(NTRIALS)); do \
		  			mpirun $(MAPFLAGS) -np $$n -x OMP_PROC_BIND=close -x OMP_NUM_THREADS=$$m src/game.x $(STATICFLAGS) -h $$s -w $$s -f $(FNAME) -n $(T) -s $(S) -t $(TFNAME); \
			  	done; \
  			done; \
	  	done; \
  	done

staticomp:
	@for s in $(SIZE); do \
		for n in $$(seq $(CPS)); do \
			for _ in $$(seq $(NTRIALS)); do \
				mpirun --map-by socket -np $$(cat /proc/cpuinfo | grep "physical id" | sort | uniq | wc -l) -x OMP_PROC_BIND=close -x OMP_NUM_THREADS=$$n src/game.x -e 1 -i -r -h $$s -w $$s -f 0 -n $(T) -t omp.txt; \
	  		done; \
	  	done; \
  	done

staticmpistrong:
	@for s in $(SIZE); do \
		for n in $$(seq $$(expr $(CPS) \* $(NS))); do \
			for _ in $$(seq $(NTRIALS)); do \
				mpirun --map-by core -np $$n -x OMP_PROC_BIND=close -x OMP_NUM_THREADS=1 src/game.x -e 1 -i -r -h $$s -w $$s -f 0 -n $(T) -t mpistrong.txt; \
			done; \
		done; \
	done

statichbstrong:
	@for s in $(SIZE); do \
		for n in $$(seq $(NS)); do \
			for _ in $$(seq $(NTRIALS)); do \
				mpirun --map-by core -np $$n -x OMP_PROC_BIND=close -x OMP_NUM_THREADS=$(CPS) src/game.x -e 1 -i -r -h $$s -w $$s -f 0 -n $(T) -t hbstrong.txt; \
			done; \
		done; \
	done

# CLEANING
.PHONY: clean
clean: clean_exe clean_snapshot clean_time

clean_exe:
	rm src/*.x

clean_snapshot:
	rm snapshot/*

clean_time:
	rm time/*

clean_sim:
	rm simulation/*