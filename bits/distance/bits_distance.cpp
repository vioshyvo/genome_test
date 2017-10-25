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
#include "distance.h"

using boost::dynamic_bitset;

using SpMatI = Eigen::SparseMatrix<int>;
using SpVecI = Eigen::SparseVector<int>;
using SpMatIRow = Eigen::SparseMatrix<int, Eigen::RowMajor>;
using T = Eigen::Triplet<int>;
using vec_bs = std::vector<dynamic_bitset<>>;
using MatI = Eigen::MatrixXi;
using VecF = Eigen::VectorXf;

int main(int argc, char** argv) {
  if(argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <seed> <n>" << std::endl;
    return -1;
  }

  int seed = atoi(argv[1]);
  size_t n = atoi(argv[2]);

  // size_t dim = 23223411;
  size_t dim = 10;
  bool verbose = dim < 50 && n < 50;

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

  MatI data_matrix;
  start = omp_get_wtime();
  generate_data_matrix(n, dim, data_density, seed, data_matrix);
  end = omp_get_wtime();
  std::cout << "Data generation time: " << end - start << std::endl;

  start = omp_get_wtime();
  SpMatI sparse_data_matrix(dim, n);
  generate_sparse_data_matrix(n, dim, data_density, seed, sparse_data_matrix);
  end = omp_get_wtime();
  std::cout << "Data generation time for sparse matrix: " << end - start << std::endl;

  start = omp_get_wtime();
  dynamic_bitset<> bitset_data(n * dim);
  generate_data_bitset(n, dim, data_density, seed, bitset_data);
  end = omp_get_wtime();
  std::cout << "Data generation time for bitset: " << end - start << std::endl;

  std::cout << std::endl;

  if(verbose) {
    std::cout << query_bitset << std::endl;
    print_Vec(query_vec);
    std::cout << "\n\n";
    print_sparse_vector(query_spvec);
    std::cout << "\n\n";
    std::cout << data_matrix << std::endl << std::endl;
    std::cout << sparse_data_matrix << std::endl;
    print(bitset_data, n , dim);
    std::cout << std::endl;
  }



  // start = omp_get_wtime();
  // VecF distance_vec(n);
  // distance(q, data_matrix, distance_vec);
  // end = omp_get_wtime();
  // std::cout << "distances:\n";
  // print_Vec(distance_vec);
  // std::cout << "Distance time for Matrix / SparseMatrix version: " << end - start << std::endl << std::endl;









  return 0;
}
