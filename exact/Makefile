CC=g++
CXX=g++
CFLAGS+=-std=c++11 -O3 -march=native -fno-rtti -fno-stack-protector -fno-exceptions -ffast-math -DNDEBUG -DEIGEN_DONT_PARALLELIZE -fopenmp -I./ -I/usr/local/include/eigen_3.3.3 -I/home/hyvi/HYVI/git/MRPT_new
LDFLAGS+=-lgomp

all:
	$(CC) -o tester tester.cpp $(CFLAGS) $(LDFLAGS)
