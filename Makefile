# Makefile structure from https://jdhao.github.io/2021/08/17/pybind11_first_impression/

CXX := g++
INCLUDE := $(shell python3 -m pybind11 --includes)
FLAG := -Wall -shared -std=c++20 -fPIC -fvisibility=hidden
SUFFIX := $(shell python3-config --extension-suffix)
DEBUG_flags := -DEMP_TRACK_MEM -g
OPT_flags := -O3 -DNDEBUG


default: opt

opt:
	$(CXX) $(FLAG) $(OPT_flags) $(INCLUDE) systematics_bindings.cpp -o phylotrackpy/systematics$(SUFFIX)

debug:

	$(CXX) $(FLAG) $(DEBUG_flags) $(INCLUDE) systematics_bindings.cpp -o phylotrackpy/systematics$(SUFFIX)

clean:
	rm -rf phylotrackpy/systematics$(SUFFIX)
