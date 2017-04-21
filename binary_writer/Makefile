EIGEN_PATH=../../eigen_3.3.3

CPPFLAGS=-I$(EIGEN_PATH)
CXXFLAGS=-O3 -std=c++11 -fexceptions -Wall
LDLIBS=-lgomp

binary_writer : ../Mrpt.h ../common.h

.PHONY: clean
clean:
	$(RM) binary_writer *.o
