EIGEN_PATH=../eigen

CPPFLAGS=-I$(EIGEN_PATH) -I../bits_utility
CXXFLAGS=-O3 -std=c++11 -fexceptions
LDLIBS=-lgomp

test_bits: ../bits_utility/distance.h ../bits_utility/project.h ../bits_utility/utility.h

.PHONY: clean
clean:
	$(RM) test_bits *.o
