#ifndef DISTANCE_H_
#define DISTANCE_H_

#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <boost/dynamic_bitset.hpp>
#include <Eigen/Sparse>
#include <Eigen/Dense>

using SpVecI = Eigen::SparseVector<int>;
using VecI = Eigen::VectorXi;
using InIterVec = SpVecI::InnerIterator;
using MatI = Eigen::MatrixXi;
using VecF = Eigen::VectorXf;


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

void distance2(const VecI &q, const MatI &X, VecI &distances) {
  int n = X.cols();
  for(int i = 0; i < n; ++i)
    distances[i] = (X.col(i) - q).squaredNorm();
}

int distance3(const VecI &x, const VecI &y) {
  return (x - y).lpNorm<1>();
}

void distance3(const VecI &q, const MatI &X, VecI &distances){
  int n = X.cols();
  for(int i = 0; i < n; ++i)
    distances[i] = (X.col(i) - q).lpNorm<1>();
}

int distance4(const VecI &x, const VecI &y) {
  return (x - y).count();
}

void distance4(const VecI &q, const MatI &X, VecI &distances) {
  int n = X.cols();
  for(int i = 0; i < n; ++i)
    distances[i] = (X.col(i) - q).count();
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

#endif
