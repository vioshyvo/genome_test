#include <iostream>
#include <random>
#include <bitset>
#include <cstdlib>
#include <vector>
#include <cmath>

#define N 10

int distance(std::bitset<N> x, std::bitset<N> y) {
  return (x ^ y).count();
}

int distance(std::vector<int> x, std::vector<int> y) {
  int sum = 0;
  size_t n = x.size();
  for(int i = 0; i < n; ++i) sum += std::abs(x[i] - y[i]);
  return sum;
}

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "Usage " << argv[0] << " <seed>" << std::endl;
    return -1;
  }

  int seed = atoi(argv[1]);
  double prob1 = 0.5;
  std::mt19937 gen(seed);
  std::bernoulli_distribution dist(prob1);
  std::bitset<N> input1, input2;
  std::vector<int> vector1(N), vector2(N);

  for(int i = 0; i < N; ++i) {
    bool v1 = dist(gen);
    bool v2 = dist(gen);
    input1.set(i, v1);
    input2.set(i, v2);
    vector1[i] = v1;
    vector2[i] = v2;
  }

  std::cout << input1 << std::endl;
  std::cout << input2 << std::endl;
  std::bitset<N> xorset = input1 ^ input2;

  std::cout << xorset << std::endl;
  std::cout << "distance: " << distance(input1, input2) << std::endl << std::endl;

  for(int i = 0; i < N; ++i) std::cout << vector1[i];
  std::cout << std::endl;
  for(int i = 0; i < N; ++i) std::cout << vector2[i];
  std::cout << std::endl;
  std::cout << "distance: " << distance(vector1, vector2) << std::endl; 


  return 0;
}
