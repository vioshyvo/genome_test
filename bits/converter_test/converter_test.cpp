#include <iostream>
#include <boost/dynamic_bitset.hpp>

using boost::dynamic_bitset;

int main(int argc, char** argv) {
  float x[] = {1, 5, 3.4, 6.9, -40.5};
  int n = 5;
  for(int i = 0; i < n; ++i) std::cout << x[i] << " ";
  std::cout << std::endl;
  return 0;
}
