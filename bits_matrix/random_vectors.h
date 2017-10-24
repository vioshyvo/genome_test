#include <random>
#include <boost/dynamic_bitset.hpp>
#include <Eigen/Dense>
#include <Eigen/Sparse>

using boost::dynamic_bitset;

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
