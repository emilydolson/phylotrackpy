# Makefile structure from https://jdhao.github.io/2021/08/17/pybind11_first_impression/

CXX := g++
INCLUDE := $(shell python3 -m pybind11 --includes)
FLAG := -O3 -Wall -shared -std=c++17 -fPIC
SUFFIX := $(shell python3-config --extension-suffix)

default:
	$(CXX) $(FLAG) $(INCLUDE) systematics_bindings.cpp -o systematics$(SUFFIX)