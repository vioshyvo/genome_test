#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <boost/dynamic_bitset.hpp>
#include <Eigen/Sparse>
#include <Eigen/Dense>

using boost::dynamic_bitset;

using SpVecI = Eigen::SparseVector<int>;
using VecI = Eigen::VectorXi;
using SpMatIRow = Eigen::SparseMatrix<int, Eigen::RowMajor>;
using vec_bs = std::vector<dynamic_bitset<>>;

int project(const dynamic_bitset<> &x, const dynamic_bitset<> &rv_plus, const dynamic_bitset<> &rv_minus) {
  return (x & rv_plus).count() - (x & rv_minus).count();
}

int project(const dynamic_bitset<> &x, const vec_bs &bitset_plus, const vec_bs &bitset_minus, std::vector<int> &out_vec) {
  size_t n = bitset_plus.size();
  int proj_val;

  for(int i = 0; i < n; ++i) {
    proj_val = (x & bitset_plus[i]).count() - (x & bitset_minus[i]).count();
    out_vec.push_back(proj_val);
  }
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

SpVecI project(const SpVecI &x, const SpMatIRow &random_mat) {
  return random_mat * x;
}

int project(const VecI &x, const SpVecI &spv_plus, const SpVecI &spv_minus) {
  return spv_plus.dot(x) - spv_minus.dot(x);
}

void project(const VecI &x, const SpMatIRow &random_mat, VecI &out_vec) {
  out_vec = random_mat * x;
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
