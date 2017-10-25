#!/bin/bash
g++ -O3 -std=c++11 -I ../eigen test_bits.cpp -o test_bits -lgomp
