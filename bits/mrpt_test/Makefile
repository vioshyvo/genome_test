BASEPATH=../..
MRPT_PATH=$(BASEPATH)/../mrpt

CPPFLAGS=-I$(BASEPATH) -I$(BASEPATH)/eigen -I$(MRPT_PATH)
CXXFLAGS=-O3 -std=c++11 -fno-rtti -fno-stack-protector -fno-exceptions -ffast-math -DNDEBUG -DEIGEN_DONT_PARALLELIZE -fopenmp
LDLIBS=-lgomp

mrpt_test: $(MRPT_PATH)/cpp/Mrpt.h $(BASEPATH)/common.h

.PHONY: clean
clean:
	$(RM) mrpt_test *.o
