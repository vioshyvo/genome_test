#ifndef RANDOM_VECTORS_H_
#define RANDOM_VECTORS_H_

#include <vector>
#include <random>
#include <boost/dynamic_bitset.hpp>
#include <Eigen/Dense>
#include <Eigen/Sparse>

using boost::dynamic_bitset;

using SpMatIRow = Eigen::SparseMatrix<int, Eigen::RowMajor>;
using T = Eigen::Triplet<int>;
using vec_bs = std::vector<dynamic_bitset<>>;
using MatI = Eigen::MatrixXi;
using SpMatI = Eigen::SparseMatrix<int>;


void build_random_bitset(int n_pool, int dim, double density, int seed,
                         vec_bs &out_plus, vec_bs &out_minus) {
  std::mt19937 gen(seed);
  std::uniform_real_distribution<float> uni_dist(0, 1);
  std::bernoulli_distribution coinflip(0.5);
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

void generate_data_bitset(int n, int dim, double data_density, int seed, std::vector<dynamic_bitset<>> &out_vec) {
  std::mt19937 gen(seed);
  std::bernoulli_distribution data_dist(data_density);
  dynamic_bitset<> x(dim);

  for(int i = 0; i < n; ++i) {
    x.reset();

    for(int j = 0; j < dim; ++j)
      if(data_dist(gen)) x.set(j);

    out_vec.push_back(x);
  }


}

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

void build_random_vectors(int n_pool, int dim, double density, int seed,
                          std::vector<std::vector<int>> &random_vectors_plus,
                          std::vector<std::vector<int>> &random_vectors_minus) {
  std::mt19937 gen(seed);
  std::uniform_real_distribution<float> uni_dist(0, 1);
  std::bernoulli_distribution coinflip(0.5);
  int rval;

  for(int j = 0; j < n_pool; ++j) {
    std::vector<int> vec_plus;
    std::vector<int> vec_minus;
    for(int i = 0; i < dim; ++i) {
      if(uni_dist(gen) > density) continue;
      if(coinflip(gen)) {
        vec_plus.push_back(i);
      } else {
        vec_minus.push_back(i);
      }
    }
    random_vectors_plus.push_back(vec_plus);
    random_vectors_minus.push_back(vec_minus);
  }



}

void generate_sparse_data_matrix(int n, int dim, double data_density, int seed, SpMatI &out_mat) {
  std::mt19937 gen(seed);
  std::bernoulli_distribution data_dist(data_density);
  std::vector<T> triplets;

  for(int j = 0; j < n; ++j)
    for(int i = 0; i < dim; ++i)
      if(data_dist(gen)) triplets.push_back(T(i, j, 1));

  out_mat.setFromTriplets(triplets.begin(), triplets.end());
}

void generate_data_matrix(int n, int dim, double data_density, int seed, MatI &out_mat) {
  out_mat =  MatI::Zero(dim, n);
  std::mt19937 gen(seed);
  std::bernoulli_distribution data_dist(data_density);

  for(int j = 0; j < n; ++j)
    for(int i = 0; i < dim; ++i)
      if(data_dist(gen)) out_mat(i, j) = 1;
}





#endif // RANDOM_VECTORS_H_
