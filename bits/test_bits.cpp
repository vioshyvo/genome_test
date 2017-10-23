#include <iostream>
#include <random>
#include <bitset>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <omp.h>
#include <boost/dynamic_bitset.hpp>
#include <Eigen/Sparse>
#include <Eigen/Dense>

// #define N 133924650
// #define N 23223411
// #define N 20

using SpVecI = Eigen::SparseVector<int>;
using SpMatI = Eigen::SparseMatrix<int>;
using T = Eigen::Triplet<int>;
using InIterVec = SpVecI::InnerIterator;
using VecI = Eigen::VectorXi;

int distance(const std::vector<int> &x, const std::vector<int> &y) {
  int sum = 0;
  size_t n = x.size();
  for(size_t i = 0; i < n; ++i) {
    sum += std::abs(x[i] - y[i]);
  }
  return sum;
}

int distance(const boost::dynamic_bitset<> &x, const boost::dynamic_bitset<> &y) {
  return (x ^ y).count();
}

int distance(const std::vector<bool> &x, const std::vector<bool> &y) {
  int sum = 0;
  size_t n = x.size();
  bool xval, yval;
  for(size_t i = 0; i < n; ++i) {
    xval = x[i];
    yval = y[i];
    sum += (xval || yval) && !(xval && yval);
  }
  return sum;
}

int distance(const SpVecI &x, const SpVecI &y) {
  int sum = 0;
  InIterVec iter_x(x), iter_y(y);
  int x_idx = -1, y_idx = -1;
  if(iter_x) x_idx = iter_x.index();
  if(iter_y) y_idx = iter_y.index();

  while(iter_x || iter_y) {
    // std::cout << "x: " << iter_x.value() << ", y: " << iter_y.value() << "\n";
    if(x_idx < y_idx) {
      // std::cout << "x_idx < y_idx: " << x_idx << " < " << y_idx << "\n";
      sum += 1;
      if(iter_x) ++iter_x; else ++iter_y;
      if(iter_x) {
        x_idx = iter_x.index();
      } else if(iter_y) {
        y_idx = iter_y.index();
      }
    } else if(x_idx > y_idx) {
      // std::cout << "x_idx > y_idx: " << x_idx << " > " << y_idx << "\n";
      sum += 1;
      if(iter_y) ++iter_y; else ++iter_x;
      if(iter_y) {
        y_idx = iter_y.index();
      } else if(iter_x) {
        x_idx = iter_x.index();
      }
    } else {
      // std::cout << "x_idx == y_idx: " << x_idx << " == " << y_idx << "\n";
      if(iter_x) ++iter_x;
      if(iter_y) ++iter_y;
      if(iter_x) x_idx = iter_x.index();
      if(iter_y) y_idx = iter_y.index();
    }
  }

  return sum;
}

int distance2(const SpVecI &x, const SpVecI &y) {
  SpVecI diff = x - y;
  return diff.nonZeros();
}

int distance(const VecI &x, const VecI &y) {
  int sum = 0;
  size_t n = x.size();
  for(size_t i = 0; i < n; ++i) {
    sum += std::abs(x[i] - y[i]);
  }
  return sum;
}

int distance2(const VecI &x, const VecI &y) {
  return (x - y).squaredNorm();
}

int distance3(const VecI &x, const VecI &y) {
  return (x - y).lpNorm<1>();
}

int distance4(const VecI &x, const VecI &y) {
  return (x - y).count();
}

int distance_basic(const std::vector<int> &x, const std::vector<int> &y) {
  int sum = 0;
  auto iter_x = x.cbegin(), iter_y = y.cbegin();
  auto x_end = x.cend(), y_end = y.cend();
  int x_idx = -1, y_idx = -1;
  if(iter_x != x_end) x_idx = *iter_x;
  if(iter_y != y_end) y_idx = *iter_y;

  while(iter_x != x_end || iter_y != y_end) {
    // std::cout << "x: " << iter_x.value() << ", y: " << iter_y.value() << "\n";
    if(x_idx < y_idx) {
      // std::cout << "x_idx < y_idx: " << x_idx << " < " << y_idx << "\n";
      sum += 1;
      if(iter_x != x_end) ++iter_x; else ++iter_y;
      if(iter_x != x_end) {
        x_idx = *iter_x;
      } else if(iter_y != y_end) {
        y_idx = *iter_y;
      }
    } else if(x_idx > y_idx) {
      // std::cout << "x_idx > y_idx: " << x_idx << " > " << y_idx << "\n";
      sum += 1;
      if(iter_y != y_end) ++iter_y; else ++iter_x;
      if(iter_y != y_end) {
        y_idx = *iter_y;
      } else if(iter_x != x_end) {
        x_idx = *iter_x;
      }
    } else {
      // std::cout << "x_idx == y_idx: " << x_idx << " == " << y_idx << "\n";
      if(iter_x != x_end) ++iter_x;
      if(iter_y != y_end) ++iter_y;
      if(iter_x != x_end) x_idx = *iter_x;
      if(iter_y != y_end) y_idx = *iter_y;
    }
  }

  return sum;
}


int project(const boost::dynamic_bitset<> &x, const boost::dynamic_bitset<> &rv_plus, const boost::dynamic_bitset<> &rv_minus) {
  return (x & rv_plus).count() - (x & rv_minus).count();
}

int project(const std::vector<int> &x, const std::vector<int> &rv_plus, const std::vector<int> &rv_minus) {
  int sum = 0;
  int xval;
  size_t n = x.size();
  for(size_t i = 0; i < n; ++i) {
    xval = x[i];
    sum += (xval * rv_plus[i] - xval * rv_minus[i]);
  }
  return sum;
}

int project(const std::vector<bool> &x, const std::vector<bool> &rv_plus, const std::vector<bool> &rv_minus) {
  int sum = 0;
  bool xval;
  size_t n = x.size();
  for(size_t i = 0; i < n; ++i) {
    xval = x[i];
    sum += (xval && rv_plus[i]) - (xval && rv_minus[i]);
  }
  return sum;
}

int project(const SpVecI &x, const SpVecI &spv_plus, const SpVecI &spv_minus){
  return x.dot(spv_plus) - x.dot(spv_minus);
}

int project(const VecI &x, const SpVecI &spv_plus, const SpVecI &spv_minus) {
  return spv_plus.dot(x) - spv_minus.dot(x);
}

int project(const VecI &x, const VecI &spv_plus, const VecI &spv_minus) {
  return spv_plus.dot(x) - spv_minus.dot(x);
}

int project_basic(const std::vector<int> &x, const std::vector<int> &sv_plus, const std::vector<int> &sv_minus) {
  int sum_plus = 0, sum_minus = 0;
  size_t n_plus = sv_plus.size(), n_minus = sv_minus.size();
  for(int i = 0; i < n_plus; ++i) {
    sum_plus += x[sv_plus[i]];
  }
  for(int i = 0; i < n_minus; ++i) {
    sum_minus += x[sv_minus[i]];
  }
  return sum_plus - sum_minus;
}

int project_basic(const VecI &x, const std::vector<int> &sv_plus, const std::vector<int> &sv_minus) {
  int sum_plus = 0, sum_minus = 0;
  size_t n_plus = sv_plus.size(), n_minus = sv_minus.size();
  for(int i = 0; i < n_plus; ++i) {
    sum_plus += x[sv_plus[i]];
  }
  for(int i = 0; i < n_minus; ++i) {
    sum_minus += x[sv_minus[i]];
  }
  return sum_plus - sum_minus;
}

int project_basic3(const std::vector<int> &x, const std::vector<int> &sv_plus, const std::vector<int> &sv_minus){
  int sum_plus = 0, sum_minus = 0;
  int n_x = x.size(), n_plus = sv_plus.size(), n_minus = sv_minus.size();
  int idx_plus = 0, idx_minus = 0;
  int idx_x = 0;

  int plus_val = -1;
  int x_val = -1;
  if(idx_plus < n_plus) plus_val = sv_plus[idx_plus];
  if(idx_x < n_x) x_val = x[idx_x];

  while(idx_plus < n_plus && idx_x < n_x) {
    if(plus_val < x_val) {
      ++idx_plus;
      if(idx_plus < n_plus) plus_val = sv_plus[idx_plus];
    } else if(plus_val > x_val) {
      ++idx_x;
      if(idx_x < n_x) x_val = x[idx_x];
    } else {
      ++sum_plus;
      ++idx_x;
      ++idx_plus;
      if(idx_plus < n_plus) plus_val = sv_plus[idx_plus];
      if(idx_x < n_x) x_val = x[idx_x];
    }
  }

  idx_x = 0;
  int minus_val = -1;
  x_val = -1;
  if(idx_minus < n_minus) minus_val = sv_minus[idx_minus];
  if(idx_x < n_x) x_val = x[idx_x];

  while(idx_minus < n_minus && idx_x < n_x) {
    if(minus_val < x_val) {
      ++idx_minus;
      if(idx_minus < n_minus) minus_val = sv_minus[idx_minus];
    } else if(minus_val > x_val) {
      ++idx_x;
      if(idx_x < n_x) x_val = x[idx_x];
    } else {
      ++sum_minus;
      ++idx_x;
      ++idx_minus;
      if(idx_minus < n_minus) minus_val = sv_minus[idx_minus];
      if(idx_x < n_x) x_val = x[idx_x];
    }
  }

  return sum_plus - sum_minus;
}

int project_basic2(const std::vector<int> &x, const std::vector<int> &sv_plus, const std::vector<int> &sv_minus){
  int sum_plus = 0, sum_minus = 0;
  auto iter_plus = sv_plus.cbegin(), iter_minus = sv_minus.cbegin(), iter_x = x.cbegin();
  auto plus_end = sv_plus.cend(), minus_end = sv_minus.cend(), x_end = x.cend();

  int plus_val = -1;
  int x_val = -1;
  if(iter_plus != plus_end) plus_val = *iter_plus;
  if(iter_x != x_end) x_val = *iter_x;

  while(iter_plus != plus_end && iter_x != x_end) {
    if(plus_val < x_val) {
      ++iter_plus;
      if(iter_plus != plus_end) plus_val = *iter_plus;
    } else if(plus_val > x_val) {
      ++iter_x;
      if(iter_x != x_end) x_val = *iter_x;
    } else {
      ++sum_plus;
      ++iter_x;
      ++iter_plus;
      if(iter_plus != plus_end) plus_val = *iter_plus;
      if(iter_x != x_end) x_val = *iter_x;
    }
  }

  iter_x = x.cbegin();
  int minus_val = -1;
  x_val = -1;
  if(iter_minus != minus_end) minus_val = *iter_minus;
  if(iter_x != x_end) x_val = *iter_x;

  while(iter_minus != minus_end && iter_x != x_end) {
    if(minus_val < x_val) {
      ++iter_minus;
      if(iter_minus != minus_end) minus_val = *iter_minus;
    } else if(minus_val > x_val) {
      ++iter_x;
      if(iter_x != x_end) x_val = *iter_x;
    } else {
      ++sum_minus;
      ++iter_x;
      ++iter_minus;
      if(iter_minus != minus_end) minus_val = *iter_minus;
      if(iter_x != x_end) x_val = *iter_x;
    }
  }

  return sum_plus - sum_minus;
}


void print_sparse_vector(SpVecI sv1) {
  size_t n = sv1.size();
  for(size_t i = 0; i < n; ++i) std::cout << sv1.coeff(i) << " ";
  std::cout << std::endl;
  std::cout << sv1;
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

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "Usage " << argv[0] << " <seed>" << std::endl;
    return -1;
  }

  bool projections = true;
  bool distances = false;
  size_t N = 23223411;
  int seed = atoi(argv[1]);
  double prob1 = 0.25;
  // double density = 0.000021; // Ecoli data set
  double density = 1.0 / std::sqrt(784); // mnist data set
  // double density = 0.4;
  std::mt19937 gen(seed);
  std::bernoulli_distribution dist(prob1);
  std::bernoulli_distribution rdist(density);
  std::bernoulli_distribution coinflip(0.5);

  std::vector<int> vector1(N), vector2(N), rvector_plus(N), rvector_minus(N);
  boost::dynamic_bitset<> dbitset1(N), dbitset2(N), rdbs_plus(N), rdbs_minus(N);
  std::vector<bool> vb1(N), vb2(N), rvb_plus(N), rvb_minus(N);
  SpMatI spm(N, 4);
  std::vector<T> triplet_list;
  triplet_list.reserve(4 * N);
  VecI dv1(N), dv2(N), dpv_plus = VecI::Zero(N), dpv_minus = VecI::Zero(N);
  std::vector<int> bv1, bv2, bv_plus, bv_minus;

  bool rplus, rminus;

  for(size_t i = 0; i < N; ++i) {
    bool v1 = dist(gen);
    bool v2 = dist(gen);
    dbitset1.set(i, v1);
    dbitset2.set(i, v2);
    vector1[i] = v1;
    vector2[i] = v2;
    vb1[i] = v1;
    vb2[i] = v2;
    dv1[i] = v1;
    dv2[i] = v2;
    if(v1) {
      triplet_list.push_back(T(i, 0, v1));
      bv1.push_back(i);
    }
    if(v2) {
      triplet_list.push_back(T(i, 1, v2));
      bv2.push_back(i);
    }

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
      if(rplus){
        triplet_list.push_back(T(i, 2, rplus));
        dpv_plus[i] = rplus;
        bv_plus.push_back(i);
      }
      if(rminus) {
        triplet_list.push_back(T(i, 3, rminus));
        if(rminus) dpv_minus[i] = rminus;
        bv_minus.push_back(i);
      }
    }
  }

  spm.setFromTriplets(triplet_list.begin(), triplet_list.end());
  SpVecI spv1 = spm.col(0), spv2 = spm.col(1);
  SpVecI spv_plus = spm.col(2), spv_minus = spm.col(3);

  double start, end;

  if(distances) {
    start = omp_get_wtime();
    int dist_vecint = distance(vector1, vector2);
    end = omp_get_wtime();
    std::cout << "distance: " << dist_vecint << std::endl;
    std::cout << "time for vector<int> version: " << end - start << std::endl;

    start = omp_get_wtime();
    int dist_vecbool = distance(vb1, vb2);
    end = omp_get_wtime();
    std::cout << "distance: " << dist_vecbool << std::endl;
    std::cout << "time for vector<bool> version: " << end - start << std::endl;

    start = omp_get_wtime();
    int d_dynbs = distance(dbitset1, dbitset2);
    end = omp_get_wtime();
    std::cout << "distance: " << d_dynbs << std::endl;
    std::cout << "time for boost::dynamic_bitset version: " << end - start << std::endl;

    start = omp_get_wtime();
    int dist_eigen_sparse = distance(spv1, spv2);
    end = omp_get_wtime();
    std::cout << "distance: " << dist_eigen_sparse << std::endl;
    std::cout << "time for SparseVector<int> version: " << end - start << std::endl;

    start = omp_get_wtime();
    int dist_eigen_sparse2 = distance2(spv1, spv2);
    end = omp_get_wtime();
    std::cout << "distance: " << dist_eigen_sparse2 << std::endl;
    std::cout << "time for count SparseVector<int> version: " << end - start << std::endl;

    start = omp_get_wtime();
    int dist_eigen_dense = distance(dv1, dv2);
    end = omp_get_wtime();
    std::cout << "distance: " << dist_eigen_dense << std::endl;
    std::cout << "time for VectorXi version: " << end - start << std::endl;

    start = omp_get_wtime();
    int dist_eigen_dense2 = distance2(dv1, dv2);
    end = omp_get_wtime();
    std::cout << "distance: " << dist_eigen_dense2 << std::endl;
    std::cout << "time for squaredNorm VectorXi version: " << end - start << std::endl;

    start = omp_get_wtime();
    int dist_eigen_dense3 = distance3(dv1, dv2);
    end = omp_get_wtime();
    std::cout << "distance: " << dist_eigen_dense3 << std::endl;
    std::cout << "time for lpNorm<1> VectorXi version: " << end - start << std::endl;

    start = omp_get_wtime();
    int dist_eigen_dense4 = distance4(dv1, dv2);
    end = omp_get_wtime();
    std::cout << "distance: " << dist_eigen_dense4 << std::endl;
    std::cout << "time for count VectorXi version: " << end - start << std::endl;
    start = omp_get_wtime();

    int dist_basic = distance_basic(bv1, bv2);
    end = omp_get_wtime();
    std::cout << "distance: " << dist_basic << std::endl;
    std::cout << "time for own basic version: " << end - start << std::endl;

    std::cout << std::endl;
  }




  //////////////////////////////////////////////////////////////////////////////
  // projections

  if(projections) {
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

    start = omp_get_wtime();
    int proj_eigen_ss = project(spv1, spv_plus, spv_minus);
    end = omp_get_wtime();
    std::cout << "projected value: " << proj_eigen_ss << std::endl;
    std::cout << "projection time for SparseVector<int> / SparseVector<int> version: " << end - start << std::endl;

    start = omp_get_wtime();
    int proj_eigen_ds = project(dv1, spv_plus, spv_minus);
    end = omp_get_wtime();
    std::cout << "projected value: " << proj_eigen_ds << std::endl;
    std::cout << "projection time for VectorXi / SparseVector<int> version: " << end - start << std::endl;

    start = omp_get_wtime();
    int proj_eigen_dd = project(dv1, dpv_plus, dpv_minus);
    end = omp_get_wtime();
    std::cout << "projected value: " << proj_eigen_dd << std::endl;
    std::cout << "projection time for VectorXi / VectorXi version: " << end - start << std::endl;

    start = omp_get_wtime();
    int proj_basic = project_basic(vector1, bv_plus, bv_minus);
    end = omp_get_wtime();
    std::cout << "projected value: " << proj_basic << std::endl;
    std::cout << "projection time for vector<int> / basic version: " << end - start << std::endl;

    start = omp_get_wtime();
    int proj_dense_basic = project_basic(dv1, bv_plus, bv_minus);
    end = omp_get_wtime();
    std::cout << "projected value: " << proj_dense_basic << std::endl;
    std::cout << "projection time for VectorXi / basic version: " << end - start << std::endl;

    start = omp_get_wtime();
    int proj_basic_basic = project_basic2(bv1, bv_plus, bv_minus);
    end = omp_get_wtime();
    std::cout << "projected value: " << proj_basic_basic << std::endl;
    std::cout << "projection time for basic / basic version: " << end - start << std::endl;

    start = omp_get_wtime();
    int proj_basic_basic_idx = project_basic3(bv1, bv_plus, bv_minus);
    end = omp_get_wtime();
    std::cout << "projected value: " << proj_basic_basic_idx << std::endl;
    std::cout << "projection time for basic / basic (non_iterator) version: " << end - start << std::endl;

    std::cout << std::endl;
  }

  return 0;
}
