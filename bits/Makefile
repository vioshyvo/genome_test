EIGEN_PATH=../eigen

CPPFLAGS=-I$(EIGEN_PATH)
CXXFLAGS=-O3 -std=c++11 -fexceptions
LDLIBS=-lgomp

test_bits: distance.h project.h

.PHONY: clean
clean:
	$(RM) test_bits *.o
