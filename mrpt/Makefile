CXX=g++-8
BASEPATH=..
EIGEN_PATH=../eigen
MRPT_PATH=$(BASEPATH)

CPPFLAGS=-I$(BASEPATH)/eigen -I$(MRPT_PATH)
CXXFLAGS=-O3 -std=c++11 -fexceptions
LDLIBS=-lgomp

mrpt_comparison : $(BASEPATH)/common.h $(MRPT_PATH)/cpp/Mrpt.h

.PHONY: clean
clean:
	$(RM) mrpt_comparison *.o
