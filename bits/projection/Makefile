EIGEN_PATH=../../eigen

CPPFLAGS=-I$(EIGEN_PATH) -I../include
CXXFLAGS=-O3 -std=c++11 -fexceptions
LDLIBS=-lgomp

bits_matrix: ../include/project.h ../include/utility.h ../include/random_vectors.h

.PHONY: clean
clean:
	$(RM) bits_matrix *.o
