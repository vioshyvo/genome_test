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
  int n = atoi(argv[2]);

  // int dim = 23223411;
  int dim = 10;

  // double density = 1.0 / std::sqrt(784); // mnist data set
  // double density = 0.000021; // Ecoli data set
  double density = 0.5;

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


  MatI data_matrix = generate_data_matrix(n, dim, data_density);

  std::cout << query_bitset << std::endl;
  print_Vec(query_vec);
  std::cout << "\n\n";
  print_sparse_vector(query_spvec);
  std::cout << "\n\n";
  std::cout << data_matrix << std::endl;


  // start = omp_get_wtime();
  // VecF distance_vec(n);
  // distance(q, data_matrix, distance_vec);
  // end = omp_get_wtime();
  // std::cout << "distances:\n";
  // print_Vec(distance_vec);
  // std::cout << "Distance time for Matrix / SparseMatrix version: " << end - start << std::endl << std::endl;









  return 0;
}
