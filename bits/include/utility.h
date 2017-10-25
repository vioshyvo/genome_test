#ifndef UTILITY_H_
#define UTILITY_H_

#include <vector>
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <boost/dynamic_bitset.hpp>

using boost::dynamic_bitset;

using SpMatI = Eigen::SparseMatrix<int>;
using SpVecI = Eigen::SparseVector<int>;
using VecI = Eigen::VectorXi;

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

void print_Vec(Eigen::VectorXi v) {
  size_t n = v.size();
  for(int i = 0; i < n; ++i) std::cout << v[i] << " ";
}

template<typename T>
void print_vector(const std::vector<T> &v) {
size_t n = v.size();
  for(int i = 0; i < n; ++i) std::cout << v[i] << " ";
}

void print(dynamic_bitset<> bs, int n, int dim) {
  for(int j = 0; j < dim; ++j) {
    for(int i = 0; i < n; ++i) {
      std::cout << bs[i * dim + j] << " ";
    }
    std::cout << std::endl;
  }
}

#endif
