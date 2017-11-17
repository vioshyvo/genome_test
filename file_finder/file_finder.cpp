#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

std::vector<std::vector<int>> read_exact(std::ifstream &infile_exact) {
  std::string s;
  int ii;
  std::vector<std::vector<int>> indices;
  while(infile_exact) {
    getline(infile_exact, s);
    getline(infile_exact, s);
    std::vector<int> v;
    std::istringstream ss(s);
    while(ss >> ii) {
      v.push_back(ii);
    }
    indices.push_back(v);
  }

  return indices;
}

std::vector<int> read_permutations(std::ifstream &infile_permutations) {
  std::vector<int> permutations;
  int i;
  while(infile_permutations >> i) {
    permutations.push_back(i);
  }
  return permutations;
}

void print(std::vector<int> x) {
  for(auto it = x.begin(); it != x.end(); ++it) std::cout << *it << " ";
}

void print(std::vector<std::vector<int>> x) {
  for(int m = 0; m < x.size(); ++m) {
    std::vector<int> v(x[m]);
    for(int n = 0; n < v.size(); ++n) {
      std::cout << v[n] << " ";
    }
    std::cout << std::endl;
  }
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

  std::ifstream infile_exact(exact_file);
  if(!infile_exact) {
      std::cerr << "Error: could not open file " << exact_file << "\n";
      return -1;
  }
  std::ifstream infile_permutations(permutations_file);
  if(!infile_permutations) {
      std::cerr << "Error: could not open file " << permutations_file << "\n";
      return -1;
  }
  std::ifstream infile_list(list_file);
  if(!infile_list) {
      std::cerr << "Error: could not open file " << list_file << "\n";
      return -1;
  }


  std::vector<std::vector<int>> true_indices = read_exact(infile_exact);
  std::vector<int> permutations = read_permutations(infile_permutations);

  std::string str;
  int j = 0;
  while (infile_list && j++ < 5) {
    infile_list >> str;
    std::cout << str << std::endl;
  }

  std::cout << std::endl;

  std::cout << "permutations: ";
  print(permutations);
  std::cout << std::endl;
  std::cout << "nearest neighbors:\n";
  print(true_indices);
  std::cout << std::endl;

  return 0;
}
