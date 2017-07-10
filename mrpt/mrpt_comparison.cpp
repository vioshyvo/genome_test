#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <unsupported/Eigen/SparseExtra>
#include <typeinfo>

#include <vector>
#include <cstdio>
#include <stdint.h>
#include <omp.h>

#include <algorithm>
#include <functional>
#include <numeric>
#include <queue>
#include <random>
#include <string>
#include <memory>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cpp/Mrpt.h"
#include "../common.h"




using namespace Eigen;

int main(int argc, char **argv) {
    size_t n = atoi(argv[1]);
    size_t ntest = atoi(argv[2]);
    int k = atoi(argv[3]);
    int n_trees = atoi(argv[4]);
    int depth = atoi(argv[5]);
    size_t dim = atoi(argv[6]);
    int mmap = atoi(argv[7]);
    std::string result_path(argv[8]);
    if (!result_path.empty() && result_path.back() != '/')
      result_path += '/';

    std::string infile_path(argv[9]);
    if (!infile_path.empty() && infile_path.back() != '/')
      infile_path += '/';

    float sparsity = atof(argv[10]);

    int last_arg = 10;
    size_t n_points = n - ntest;
    bool verbose = false;


    /////////////////////////////////////////////////////////////////////////////////////////
    // test mrpt
    float *train, *test;

    test = read_memory((infile_path + "test.bin").c_str(), ntest, dim);
    if(!test) {
        std::cerr << "in mrpt_comparison: test data " << infile_path + "test.bin" << " could not be read\n";
        return -1;
    }

    if(mmap) {
        train = read_mmap((infile_path + "train.bin").c_str(), n_points, dim);
    } else {
        train = read_memory((infile_path + "train.bin").c_str(), n_points, dim);
    }

    if(!train) {
        std::cerr << "in mrpt_comparison: training data " << infile_path + "train.bin" << " could not be read\n";
        return -1;
    }


    const Map<const MatrixXf> *M = new Map<const MatrixXf>(train, dim, n_train);

    Mrpt index_dense(M, n_trees, depth, sparsity);
    index_dense.grow();

    for (int arg = last_arg + 1; arg < argc; ++arg) {
        int votes = atoi(argv[arg]);
        if (votes > n_trees) continue;

        std::vector<double> times;
        std::vector<double> projection_times, exact_times, elect_times;
        double projection_time = 0, exact_time = 0, elect_time = 0;
        std::vector<std::set<int>> idx;

        for (int i = 0; i < ntest; ++i) {
                std::vector<int> result(k);
                double start = omp_get_wtime();
                int k_found = index_dense.query(Map<VectorXf>(&test[i * dim], dim), k, votes, &result[0], projection_time, exact_time, elect_time);

                double end = omp_get_wtime();
                times.push_back(end - start);
                projection_times.push_back(projection_time);
                exact_times.push_back(exact_time);
                elect_times.push_back(elect_time);
                idx.push_back(std::set<int>(result.begin(), result.begin() + k_found)); // k_found (<= k) is the number of k-nn canditates returned
            }

        if(verbose)
            std::cout << "k: " << k << ", # of trees: " << n_trees << ", depth: " << depth << ", sparsity: " << sparsity << ", votes: " << votes << "\n";
        else
            std::cout << k << " " << n_trees << " " << depth << " " << sparsity << " " << votes << " ";

        results(k, times, idx, (result_path + "truth_" + std::to_string(k)).c_str(), verbose, projection_times, exact_times, elect_times);

    }

    delete[] test;
    if(!mmap) delete[] train;


    return 0;
}
