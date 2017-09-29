#include <iostream>
#include <random>
#include <bitset>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <omp.h>
#include <boost/dynamic_bitset.hpp>
#include <Eigen/Sparse>

// #define N 133924650
#define N 23223411
// #define N 20

using SpVec = Eigen::SparseVector<int>;
using SpMat = Eigen::SparseMatrix<int>;
using T = Eigen::Triplet<int>;

int distance(std::bitset<N> x, std::bitset<N> y) {
  return (x ^ y).count();
}

int distance(std::vector<int> x, std::vector<int> y) {
  int sum = 0;
  size_t n = x.size();
  for(int i = 0; i < n; ++i) {
    sum += std::abs(x[i] - y[i]);
  }
  return sum;
}

int distance(boost::dynamic_bitset<> x, boost::dynamic_bitset<> y) {
  return (x ^ y).count();
}

int distance(std::vector<bool> x, std::vector<bool> y) {
  int sum = 0;
  size_t n = x.size();
  bool xval, yval;
  for(int i = 0; i < n; ++i) {
    xval = x[i];
    yval = y[i];
    sum += (xval || yval) && !(xval && yval);
  }
  return sum;
}

int distance(SpVec x, SpVec y) {
  return x.dot(y);
}

int project(std::bitset<N> x, std::bitset<N> rv_plus, std::bitset<N> rv_minus) {
  return (x & rv_plus).count() - (x & rv_minus).count();
}

int project(boost::dynamic_bitset<> x, boost::dynamic_bitset<> rv_plus, boost::dynamic_bitset<> rv_minus) {
  return (x & rv_plus).count() - (x & rv_minus).count();
}

int project(std::vector<int> x, std::vector<int> rv_plus, std::vector<int> rv_minus) {
  int sum = 0;
  int xval;
  size_t n = x.size();
  for(int i = 0; i < N; ++i) {
    xval = x[i];
    sum += (xval * rv_plus[i] - xval * rv_minus[i]);
  }
  return sum;
}

int project(std::vector<bool> x, std::vector<bool> rv_plus, std::vector<bool> rv_minus) {
  int sum = 0;
  bool xval;
  size_t n = x.size();
  for(int i = 0; i < N; ++i) {
    xval = x[i];
    sum += (xval && rv_plus[i]) - (xval && rv_minus[i]);
  }
  return sum;
}

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "Usage " << argv[0] << " <seed>" << std::endl;
    return -1;
  }

  bool verbose = false;
  int seed = atoi(argv[1]);
  double prob1 = 0.5;
  double density = 0.000021;
  std::mt19937 gen(seed);
  std::bernoulli_distribution dist(prob1);
  std::bernoulli_distribution rdist(density);
  std::bernoulli_distribution coinflip(0.5);

  std::vector<int> vector1(N), vector2(N), rvector_plus(N), rvector_minus(N);
  boost::dynamic_bitset<> dbitset1(N), dbitset2(N), rdbs_plus(N), rdbs_minus(N);
  std::vector<bool> vb1(N), vb2(N), rvb_plus(N), rvb_minus(N);
  bool rvalue, rplus, rminus;

  for(int i = 0; i < N; ++i) {
    bool v1 = dist(gen);
    bool v2 = dist(gen);
    dbitset1.set(i, v1);
    dbitset2.set(i, v2);
    vector1[i] = v1;
    vector2[i] = v2;
    vb1[i] = v1;
    vb2[i] = v2;

    bool nonzero = rdist(gen);
    if(nonzero) {
      bool sign = coinflip(gen);
      rplus = sign;
      rminus = !sign;

      rdbs_plus.set(i, rplus);
      rdbs_minus.set(i, rminus);
      rvector_plus[i] = rplus;
      rvector_minus[i] = rminus;
      rvb_plus[i] = rplus;
      rvb_minus[i] = rminus;
    }
  }


  // if(verbose) {
  //   for(int i = 0; i < N; ++i) std::cout << vector1[i];
  //   std::cout << std::endl;
  //   for(int i = 0; i < N; ++i) std::cout << vector2[i];
  //   std::cout << std::endl;
  //  }


  auto start = omp_get_wtime();
  int d2 = distance(vector1, vector2);
  auto end = omp_get_wtime();
  std::cout << "distance: " << d2 << std::endl;
  std::cout << "time for vector<int> version: " << end - start << std::endl;

  start = omp_get_wtime();
  int d4 = distance(vb1, vb2);
  end = omp_get_wtime();
  std::cout << "distance: " << d4 << std::endl;
  std::cout << "time for vector<bool> version: " << end - start << std::endl;

  start = omp_get_wtime();
  int d3 = distance(dbitset1, dbitset2);
  end = omp_get_wtime();
  std::cout << "distance: " << d3 << std::endl;
  std::cout << "time for boost::dynamic_bitset version: " << end - start << std::endl;

  std::cout << std::endl;

  start = omp_get_wtime();
  int proj_vec = project(vector1, rvector_plus, rvector_minus);
  end = omp_get_wtime();
  std::cout << "projected value: " << proj_vec << std::endl;
  std::cout << "projection time for vector<int> version: " << end - start << std::endl;

  start = omp_get_wtime();
  int proj_vbool = project(vb1, rvb_plus, rvb_minus);
  end = omp_get_wtime();
  std::cout << "projected value: " << proj_vbool << std::endl;
  std::cout << "projection time for vector<bool> version: " << end - start << std::endl;

  start = omp_get_wtime();
  int proj_dbs = project(dbitset1, rdbs_plus, rdbs_minus);
  end = omp_get_wtime();
  std::cout << "projected value: " << proj_dbs << std::endl;
  std::cout << "projection time for dynamic_bitset version: " << end - start << std::endl;

  std::cout << std::endl;

  return 0;
}
