#include <iostream>
#include <random>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <omp.h>
#include <cmath>
#include <boost/dynamic_bitset.hpp>
#include "project.h"
#include "utility.h"

using boost::dynamic_bitset;

using SpMatI = Eigen::SparseMatrix<int>;
using SpVecI = Eigen::SparseVector<int>;
using SpMatIRow = Eigen::SparseMatrix<int, Eigen::RowMajor>;
using T = Eigen::Triplet<int>;
using vec_bs = std::vector<dynamic_bitset<>>;

void build_random_bitset(int n_pool, int dim, double density, int seed,
                         vec_bs &out_plus, vec_bs &out_minus) {
  std::mt19937 gen(seed);
  std::uniform_real_distribution<float> uni_dist(0, 1);
  std::bernoulli_distribution coinflip(0.5);
  int rval;
  dynamic_bitset<> bitset_plus(dim), bitset_minus(dim);

  for(int i = 0; i < n_pool; ++i) {
    bitset_plus.reset();
    bitset_minus.reset();
    for(int j = 0; j < dim; ++j) {
      if(uni_dist(gen) > density) continue;
      if(coinflip(gen)) bitset_plus.set(j); else bitset_minus.set(j);
    }
    out_plus.push_back(bitset_plus);
    out_minus.push_back(bitset_minus);
  }
};

SpMatIRow build_sparse_random_matrix(int n_pool, int dim, double density, int seed) {
        SpMatIRow sparse_random_matrix(n_pool, dim);

        std::mt19937 gen(seed);
        std::uniform_real_distribution<float> uni_dist(0, 1);
        std::bernoulli_distribution coinflip(0.5);
        int rval;

        std::vector<T> triplets;
        for (int j = 0; j < n_pool; ++j) {
            for (int i = 0; i < dim; ++i) {
                if (uni_dist(gen) > density) continue;
                rval = coinflip(gen) ? 1 : -1;
                triplets.push_back(T(j, i, rval));
            }
        }

        sparse_random_matrix.setFromTriplets(triplets.begin(), triplets.end());
        sparse_random_matrix.makeCompressed();
        return sparse_random_matrix;
}

int main(int argc, char** argv) {
  if(argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <seed> <n>" << std::endl;
    return -1;
  }

  int seed = atoi(argv[1]);
  int n_pool = atoi(argv[2]);
  int dim = 23223411;
  // int dim = 10;
  double density = 1.0 / std::sqrt(784); // mnist data set
  // double density = 0.5;
  double data_density = 0.25;
  std::mt19937 gen(seed);
  std::bernoulli_distribution data_dist(data_density);

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

  // std::cout << sparse_random_matrix << std::endl;
  // print_vector(random_bitset_plus);
  // std::cout << std::endl;
  // print_vector(random_bitset_minus);
  // std::cout << std::endl;
  //
  // std::cout << std::endl;
  // print_VecI(query_vec);
  // std::cout << std::endl;
  // std::cout << query_bitset << std::endl << std::endl;

  double start, end;

  start = omp_get_wtime();
  SpVecI proj_spvec = project(query_spvec, sparse_random_matrix);
  end = omp_get_wtime();
  std::cout << "projected values:\n";
  print_sparse_vector(proj_spvec);
  std::cout << "\nprojection time for SparseVector / SparseMatrix version: " << end - start << std::endl << std::endl;

  start = omp_get_wtime();
  VecI proj_vec = project(query_vec, sparse_random_matrix);
  end = omp_get_wtime();
  std::cout << "projected values:\n";
  print_VecI(proj_vec);
  std::cout << "\nprojection time for Vector / SparseMatrix version: " << end - start << std::endl << std::endl;

  start = omp_get_wtime();
  VecI proj_vec2;
  project2(query_vec, sparse_random_matrix, proj_vec2);
  end = omp_get_wtime();
  std::cout << "projected values:\n";
  print_VecI(proj_vec2);
  std::cout << "\nprojection time for inline Vector / SparseMatrix version: " << end - start << std::endl << std::endl;

  start = omp_get_wtime();
  std::vector<int> proj_vec_dbs;
  project(query_bitset, random_bitset_plus, random_bitset_minus, proj_vec_dbs);
  end = omp_get_wtime();
  std::cout << "projected values:\n";
  print_vector(proj_vec_dbs);
  std::cout << "\nprojection time for bitset / bitset version: " << end - start << std::endl << std::endl;










  return 0;
}
