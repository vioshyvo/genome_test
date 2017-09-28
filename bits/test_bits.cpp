#include <iostream>
#include <random>
#include <bitset>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <omp.h>
#include <boost/dynamic_bitset.hpp>

#define N 100000

int distance(std::bitset<N> x, std::bitset<N> y) {
  return (x ^ y).count();
}

int distance(std::vector<int> x, std::vector<int> y) {
  int sum = 0;
  size_t n = x.size();
  for(int i = 0; i < n; ++i) sum += std::abs(x[i] - y[i]);
  return sum;
}

int distance(boost::dynamic_bitset<> x, boost::dynamic_bitset<> y) {
  return (x ^ y).count();
}


int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "Usage " << argv[0] << " <seed>" << std::endl;
    return -1;
  }

  bool verbose = false;
  int seed = atoi(argv[1]);
  double prob1 = 0.5;
  std::mt19937 gen(seed);
  std::bernoulli_distribution dist(prob1);

  std::bitset<N> input1, input2;
  std::vector<int> vector1(N), vector2(N);
  boost::dynamic_bitset<> dbitset1(N), dbitset2(N); // all 0's by default

  for(int i = 0; i < N; ++i) {
    bool v1 = dist(gen);
    bool v2 = dist(gen);
    input1.set(i, v1);
    input2.set(i, v2);
    dbitset1.set(i, v1);
    dbitset2.set(i, v2);
    vector1[i] = v1;
    vector2[i] = v2;
  }

  if(verbose) {
    std::cout << input1 << std::endl;
    std::cout << input2 << std::endl;
    std::bitset<N> xorset = input1 ^ input2;
    std::cout << xorset << std::endl;
  }


  auto start = omp_get_wtime();
  int d1 = distance(input1, input2);
  auto end = omp_get_wtime();

  std::cout << "distance: " << d1 << std::endl;
  std::cout << "time for bitset version: " << end - start << std::endl;

  if(verbose) {
    for(int i = 0; i < N; ++i) std::cout << vector1[i];
    std::cout << std::endl;
    for(int i = 0; i < N; ++i) std::cout << vector2[i];
    std::cout << std::endl;
   }

  start = omp_get_wtime();
  int d2 = distance(vector1, vector2);
  end = omp_get_wtime();
  std::cout << "distance: " << d2 << std::endl;
  std::cout << "time for vector<int> version: " << end - start << std::endl;

  start = omp_get_wtime();
  int d3 = distance(dbitset1, dbitset2);
  end = omp_get_wtime();
  std::cout << "distance: " << d3 << std::endl;
  std::cout << "time for boost::dynamic_bitset version: " << end - start << std::endl;

  return 0;
}
