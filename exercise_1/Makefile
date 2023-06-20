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
TFNAME := time/time.txt

CPS := 8

# COMPILATION FLAGS
COMPILEFLAGS := -O2 -Wall -Wstrict-aliasing -fopenmp
SAVINGFLAGS :=

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

execute:
	chmod +x ordered.sh
	chmod +x static.sh

# ORDERED RUN
ordered:
#	@for s in $(SIZE); do \
#		for _ in $$(seq $(NTRIALS)); do \
#			mpirun -np 1 -x OMP_NUM_THREADS=1 src/game.x $(ORDEREDFLAGS) -h $$s -w $$s -f $(FNAME) -n $(T) -s $(S) >> $(TFNAME); \
#		done; \
#	done
	./ordered.sh

# STATIC RUN
static:
#	@for s in $(SIZE); do \
#  		for n in $(NTASKS); do \
#  			for m in $(NTHREADS); do \
#		  		for _ in $$(seq $(NTRIALS)); do \
#		  			mpirun $(MAPFLAGS) -np $$n -x OMP_PROC_BIND=close -x OMP_NUM_THREADS=$$m src/game.x $(STATICFLAGS) -h $$s -w $$s -f $(FNAME) -n $(T) -s $(S) >> $(TFNAME); \
#			  	done; \
#  			done; \
#	  	done; \
#  	done
	./static.sh

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