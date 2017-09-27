#include <iostream>
#include <random>
#include <bitset>
#include <cstdlib>

#define N 10

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "Usage " << argv[0] << " <seed>" << std::endl;
    return -1;
  }

  int seed = atoi(argv[1]);
  double prob1 = 0.25;
  std::mt19937 gen(seed);
  std::bernoulli_distribution dist(prob1);
  std::bitset<N> input1, input2;

  for(int i = 0; i < N; ++i) {
    input1.set(i, dist(gen));
    input2.set(i, dist(gen));
  }

  std::cout << input1 << std::endl;
  std::cout << input2 << std::endl;

  return 0;
}
