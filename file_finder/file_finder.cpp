#include <iostream>
#include <string>

int main(int argc, char **argv) {
  int n_args = 1;
  if (argc != n_args + 1) {
      std::cerr << "Usage: " << argv[0] << " permutations_file" << std::endl;
      return 1;
  }
  
  std::string permutations_file(argv[1]);
  std::cout << "Hi from C++!" << std::endl;


  return 0;
}
