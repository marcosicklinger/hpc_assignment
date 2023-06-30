MAP := "socket"
SIZE := "100"
NTASKS := "1"
NTHREADS := "1 2 4 8 16"

# COMPILATION FLAGS
COMPILEFLAGS := -O2 -Wall -Wstrict-aliasing -march=native -fopenmp
SAVINGFLAGS :=

# SETUP OF DIRECTORY
setup:
	@mkdir -p src/exe
	@mkdir -p snapshot
	@mkdir -p time
	@mkdir -p simulation

# COMPILE APPLICATION
game: src/exe/game.x

src/exe/game.x: src/app/game.cpp src/lib/Life.cpp src/lib/utils.cpp
	@mpicc $(COMPILEFLAGS) $(SAVINGFLAGS) -o $@ $^ -lstdc++

execute:
	@chmod +x ordered.sh
	@chmod +x static.sh

# ORDERED RUN
ordered:
#	@for s in $(SIZE); do \
#		for _ in $$(seq $(NTRIALS)); do \
#			mpirun -np 1 -x OMP_NUM_THREADS=1 src/game.x $(ORDEREDFLAGS) -h $$s -w $$s -f $(FNAME) -n $(T) -s $(S) >> $(TFNAME); \
#		done; \
#	done
	./ordered.sh -k "$(SIZE)"

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
	./static.sh -m "$(MAP)" -k "$(SIZE)" -p "$(NTASKS)" -t "$(NTHREADS)"

# CLEANING
.PHONY: clean
clean: clean_exe clean_snapshot clean_time

clean_exe:
	@rm src/exe/*.x

clean_snapshot:
	@rm snapshot/*

clean_time:
	@rm time/*

clean_sim:
	@rm simulation/*