EIGEN_PATH=../eigen

CPPFLAGS=-I$(EIGEN_PATH) -I../bits
CXXFLAGS=-O3 -std=c++11 -fexceptions
LDLIBS=-lgomp

bits_matrix: ../bits/project.h ../bits/utility.h

.PHONY: clean
clean:
	$(RM) bits_matrix *.o
