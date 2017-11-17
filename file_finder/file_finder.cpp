#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char **argv) {
  int n_args = 2;
  if (argc != n_args + 1) {
      std::cerr << "Usage: " << argv[0] << " permutations_file exact_file" << std::endl;
      return 1;
  }

  std::string permutations_file(argv[1]);
  std::string exact_file(argv[2]);
  std::cout << "Hi from C++!" << std::endl;

  std::ifstream infile(permutations_file);
  if(!infile) {
      std::cerr << "Error: could not open file " << permutations_file << "\n";
      return -1;
  }

  int i;
  while(infile) {
    infile >> i;
    std::cout << i << " ";
  }
  std::cout << std::endl;

  std::ifstream infile_exact(exact_file);
  if(!infile_exact) {
      std::cerr << "Error: could not open file " << permutations_file << "\n";
      return -1;
  }

  std::string s;
  while(infile_exact) {
    infile_exact >> s;
    std::cout << s << " ";
  }

  std::cout << std::endl;
  return 0;
}
