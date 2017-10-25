#include <iostream>
#include <random>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <omp.h>
#include <boost/dynamic_bitset.hpp>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include "distance.h"
#include "project.h"
#include "utility.h"

using SpVecI = Eigen::SparseVector<int>;
using SpMatI = Eigen::SparseMatrix<int>;
using T = Eigen::Triplet<int>;
using InIterVec = SpVecI::InnerIterator;
using VecI = Eigen::VectorXi;

int main(int argc, char **argv) {
  if(argc != 2) {
    std::cerr << "Usage " << argv[0] << " <seed>" << std::endl;
    return -1;
  }

  bool projections = true;
  bool distances = true;
  size_t N = 23223411;
  int seed = atoi(argv[1]);
  double data_density = 0.25;
  double density = 0.000021; // Ecoli data set
  // double density = 1.0 / std::sqrt(784); // mnist data set
  // double density = 0.4;
  std::mt19937 gen(seed);
  std::bernoulli_distribution dist(data_density);
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

    std::cout << std::endl;
  }

  return 0;
}
