#include <iostream>
#include <random>
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <omp.h>
#include <cmath>
#include "project.h"
#include "utility.h"

using SpMatI = Eigen::SparseMatrix<int>;
using SpVecI = Eigen::SparseVector<int>;
using SpMatIRow = Eigen::SparseMatrix<int, Eigen::RowMajor>;
using T = Eigen::Triplet<int>;

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
  double density = 1.0 / std::sqrt(784); // mnist data set
  double data_density = 0.25;
  std::mt19937 gen(seed);
  std::bernoulli_distribution data_dist(data_density);

  std::vector<T> triplets2;
  SpMatI spmat(dim, 1);
  for(int i = 0; i < dim; ++i) {
    if(data_dist(gen)) triplets2.push_back(T(i, 0, 1));
  }

  spmat.setFromTriplets(triplets2.begin(), triplets2.end());
  SpVecI query_spvec = spmat.col(0);

  SpMatIRow sparse_random_matrix = build_sparse_random_matrix(n_pool, dim, density, seed);

  double start, end;

  start = omp_get_wtime();
  SpVecI proj_spvec = project(query_spvec, sparse_random_matrix);
  end = omp_get_wtime();
  std::cout << "projected values:\n";
  print_sparse_vector(proj_spvec);
  std::cout << "\n\nprojection time for vector<int> version: " << end - start << std::endl;





















  return 0;
}
