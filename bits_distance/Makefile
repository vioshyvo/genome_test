EIGEN_PATH=../eigen

CPPFLAGS=-I$(EIGEN_PATH) -I../bits_utility
CXXFLAGS=-O3 -std=c++11 -fexceptions
LDLIBS=-lgomp

bits_distance: ../bits_utility/distance.h ../bits_utility/utility.h ../bits_utility/random_vectors.h

.PHONY: clean
clean:
	$(RM) bits_distance *.o
