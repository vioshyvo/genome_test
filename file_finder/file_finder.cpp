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

std::vector<std::string> read_names(std::ifstream &infile_list){
  std::vector<std::string> file_names;
  std::string str;
  int j = 0;
  while (infile_list >> str) {
    file_names.push_back(str);
  }
  return file_names;
}

void split_permutations(const std::vector<int> &permutations, std::vector<int> &permutations_test,
   std::vector<int> &permutations_train, int n_train, int n_test) {
   int n = permutations.size();
   int i = 0;
   for(; i < n_test; ++i) permutations_test.push_back(permutations[i]);
   for(; i < n; ++i) permutations_train.push_back(permutations[i]);
}


void print(const std::vector<int> &x) {
  for(auto it = x.begin(); it != x.end(); ++it) std::cout << *it << " ";
}

void head(const std::vector<std::string> &v, int n = 5) {
  for(int i = 0; i < v.size() && i < n; ++i) std::cout << v[i] << "\n";
}

void tail(const std::vector<std::string> &v, int n = 5) {
  int start = v.size() > n ? v.size() - n : 0;
  for(int i = start; i < v.size(); ++i) std::cout << v[i] << "\n";
}

void print(const std::vector<std::vector<int>> &x) {
  for(int m = 0; m < x.size(); ++m) {
    std::vector<int> v(x[m]);
    for(int n = 0; n < v.size(); ++n) {
      std::cout << v[n] << " ";
    }
    std::cout << std::endl;
  }
}

void print_vectors(const std::vector<int> &permutations, const std::vector<std::vector<int>> &true_indices,
  const std::vector<std::string> &file_names, int n_train, int n_test,
  const std::vector<int> &permutations_train, const std::vector<int> &permutations_test) {
    std::cout << "\n";
    std::cout << "n_train: " << n_train << "\n";
    std::cout << "n_test: " << n_test << "\n\n";

    std::cout << "permutations: ";
    print(permutations);
    std::cout << "\n";
    std::cout << "permutations of test set: ";
    print(permutations_test);
    std::cout << "\n";
    std::cout << "permutations of training set: ";
    print(permutations_train);
    std::cout << "\n\n";

    std::cout << "nearest neighbors:\n";
    print(true_indices);
    std::cout << "\n";

    std::cout << "first file names:\n";
    head(file_names);
    std::cout << "\n";
    std::cout << "last file names:\n";
    tail(file_names);
    std::cout << "\n";
}

int main(int argc, char **argv) {
  int n_args = 6;
  if (argc != n_args + 1) {
      std::cerr << "Usage: " << argv[0] << " permutations_file exact_file list_file n_train n_test res_dir" << std::endl;
      return 1;
  }

  std::string permutations_file(argv[1]);
  std::string exact_file(argv[2]);
  std::string list_file(argv[3]);
  int n_train = atoi(argv[4]);
  int n_test = atoi(argv[5]);

  std::string res_dir(argv[6]);
  if (!res_dir.empty() && res_dir.back() != '/')
    res_dir += '/';

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
  std::vector<std::string> file_names = read_names(infile_list);

  std::vector<int> permutations_test, permutations_train;
  split_permutations(permutations, permutations_test, permutations_train, n_train, n_test);

  print_vectors(permutations, true_indices, file_names, n_train, n_test,
    permutations_train, permutations_test);

  for(int i = 0; i < permutations_test.size(); ++i) {
    int true_index = permutations_test[i];
    std::string oname(file_names[true_index]);
    std::cout << true_index << " : " << oname << "\n";
    std::vector<int> nn = true_indices[i];

    for(int j = 0; j < nn.size(); ++j){
      std::cout << file_names[permutations_train[nn[j]]] << "\n";
    }
    std::cout << "\n";
  }

  std::cout << "\n";

  return 0;
}
