#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

std::vector<std::vector<int>> read_exact(std::string exact_file) {
  std::ifstream infile_exact(exact_file);
  if(!infile_exact) {
      std::cerr << "Error: could not open file " << exact_file << "\n";
      return std::vector<std::vector<int>>();
  }

  std::string s;
  int ii;
  std::vector<std::vector<int>> indices;
  while(infile_exact) {
    getline(infile_exact, s);
    getline(infile_exact, s);
    std::vector<int> v;
    std::istringstream ss(s);
    while(ss >> ii) {
      std::cout << ii << " ";
      v.push_back(ii);
    }
    indices.push_back(v);
    std::cout << std::endl;
  }
  std::cout << std::endl;

  return indices;
}

int main(int argc, char **argv) {
  int n_args = 3;
  if (argc != n_args + 1) {
      std::cerr << "Usage: " << argv[0] << " permutations_file exact_file list_file" << std::endl;
      return 1;
  }

  std::string permutations_file(argv[1]);
  std::string exact_file(argv[2]);
  std::string list_file(argv[3]);

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



  std::ifstream infile_list(list_file);
  if(!infile_list) {
      std::cerr << "Error: could not open file " << list_file << "\n";
      return -1;
  }

  std::string str;
  int j = 0;
  while (infile_list && j++ < 5) {
    infile_list >> str;
    std::cout << str << std::endl;
  }

  std::cout << std::endl;

  std::vector<std::vector<int>> true_indices = read_exact(exact_file);
  for(int m = 0; m < true_indices.size(); ++m) {
    std::vector<int> v(true_indices[m]);
    for(int n = 0; n < v.size(); ++n) {
      std::cout << v[n] << " ";
    }
    std::cout << std::endl;
  }

  return 0;
}
