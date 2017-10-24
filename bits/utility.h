#include<iostream>
#include<Eigen/Dense>
#include<Eigen/Sparse>

using SpMatI = Eigen::SparseMatrix<int>;
using SpVecI = Eigen::SparseVector<int>;

void print_sparse_vector(SpVecI sv1) {
  size_t n = sv1.size();
  for(size_t i = 0; i < n; ++i) std::cout << sv1.coeff(i) << " ";
}

void print_inner_iterator(SpVecI sv1) {
  for(SpVecI::InnerIterator it(sv1); it; ++it) {
    std::cout << "index: " << it.index() << ", ";
    std::cout << "value: " << it.value() << std::endl;
  }
}

void print(SpVecI sv1) {
  print_sparse_vector(sv1);
  std::cout << std::endl;
  print_inner_iterator(sv1);
  std::cout << std::endl;
}
