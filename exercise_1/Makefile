# COMPILATION FLAGS
# -----------------
COMPILEFLAGS := -O2 -Wall -Wstrict-aliasing -march=native -fopenmp
SAVINGFLAGS :=
# -----------------

# SETUP OF DIRECTORY
# ------------------
setup:
	@mkdir -p src/exe
	@mkdir -p snapshot
	@mkdir -p time
	@mkdir -p simulation
# ------------------

# COMPILE APPLICATION
# -------------------
game: src/exe/game.x

src/exe/game.x: src/app/game.cpp src/lib/Life.cpp src/lib/utils.cpp
	@mpicc $(COMPILEFLAGS) $(SAVINGFLAGS) -o $@ $^ -lstdc++
# -------------------

# EXECUTION
# ---------
executable:
	@chmod +x ordered.sh
	@chmod +x mpiscal.sh
	@chmod +x ompscal.sh
# ---------

# CLEANING
# --------
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
# --------