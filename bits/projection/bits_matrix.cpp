#include <vector>
#include <iostream>
#include <random>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <omp.h>
#include <cmath>
#include <boost/dynamic_bitset.hpp>
#include "project.h"
#include "utility.h"
#include "random_vectors.h"

using boost::dynamic_bitset;

using SpMatI = Eigen::SparseMatrix<int>;
using SpVecI = Eigen::SparseVector<int>;
using SpMatIRow = Eigen::SparseMatrix<int, Eigen::RowMajor>;
using T = Eigen::Triplet<int>;
using vec_bs = std::vector<dynamic_bitset<>>;

int main(int argc, char** argv) {
  if(argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <seed> <n>" << std::endl;
    return -1;
  }

  int seed = atoi(argv[1]);
  int n_pool = atoi(argv[2]);

  int dim = 23223411;
  // int dim = 10;

  // double density = 1.0 / std::sqrt(784); // mnist data set
  double density = 0.000021; // Ecoli data set
  // double density = 0.5;

  double data_density = 0.25;
  std::mt19937 gen(seed);
  std::bernoulli_distribution data_dist(data_density);
  double start, end;

  VecI query_vec = VecI::Zero(dim);
  dynamic_bitset<> query_bitset(dim);
  query_bitset.reset();

  std::vector<T> triplets2;
  SpMatI spmat(dim, 1);
  for(int i = 0; i < dim; ++i) {
    if(data_dist(gen)) {
      triplets2.push_back(T(i, 0, 1));
      query_vec[i] = 1;
      query_bitset.set(i);
    }
  }

  spmat.setFromTriplets(triplets2.begin(), triplets2.end());
  SpVecI query_spvec = spmat.col(0);

  SpMatIRow sparse_random_matrix = build_sparse_random_matrix(n_pool, dim, density, seed);
  vec_bs random_bitset_plus, random_bitset_minus;
  build_random_bitset(n_pool, dim, density, seed, random_bitset_plus, random_bitset_minus);

  start = omp_get_wtime();
  SpVecI proj_spvec;
  project(query_spvec, sparse_random_matrix, proj_spvec);
  end = omp_get_wtime();
  std::cout << "projected values:\n";
  print_sparse_vector(proj_spvec);
  std::cout << "\nprojection time for SparseVector / SparseMatrix version: " << end - start << std::endl << std::endl;

  start = omp_get_wtime();
  VecI proj_vec;
  project(query_vec, sparse_random_matrix, proj_vec);
  end = omp_get_wtime();
  std::cout << "projected values:\n";
  print_Vec(proj_vec);
  std::cout << "\nprojection time for Vector / SparseMatrix version: " << end - start << std::endl << std::endl;

  start = omp_get_wtime();
  std::vector<int> proj_vec_dbs;
  project(query_bitset, random_bitset_plus, random_bitset_minus, proj_vec_dbs);
  end = omp_get_wtime();
  std::cout << "projected values:\n";
  print_vector(proj_vec_dbs);
  std::cout << "\nprojection time for bitset / bitset version: " << end - start << std::endl << std::endl;










  return 0;
}
