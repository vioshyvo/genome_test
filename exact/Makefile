CXX=g++-8
EIGEN_PATH=../eigen
MRPT_PATH=..

CPPFLAGS=-I$(EIGEN_PATH) -I$(MRPT_PATH) -I./
CXXFLAGS=-O3 -std=c++11 -march=native -fno-rtti -fno-stack-protector -fno-exceptions -ffast-math -DNDEBUG -DEIGEN_DONT_PARALLELIZE -fopenmp
LDLIBS=-lgomp

tester : ../common.h

.PHONY: clean
clean:
	$(RM) tester *.o
