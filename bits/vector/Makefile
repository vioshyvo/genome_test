EIGEN_PATH=../../eigen

CPPFLAGS=-I$(EIGEN_PATH) -I../include
CXXFLAGS=-O3 -std=c++11 -fexceptions
LDLIBS=-lgomp

test_bits: ../include/distance.h ../include/project.h ../include/utility.h

.PHONY: clean
clean:
	$(RM) test_bits *.o
