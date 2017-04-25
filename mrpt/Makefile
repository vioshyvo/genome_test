EIGEN_PATH=../../eigen_3.3.3
MRPT_PATH=..

CPPFLAGS=-I$(EIGEN_PATH) -I$(MRPT_PATH)
CXXFLAGS=-O3 -std=c++11 -fexceptions -Wall
LDLIBS=-lgomp

mrpt_comparison : ../common.h

.PHONY: clean
clean:
	$(RM) mrpt_comparison *.o
