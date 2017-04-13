#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <Eigen/SparseCore>
#include <bench/BenchTimer.h>
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
    int n = atoi(argv[1]);
    int n_test = atoi(argv[2]);
    int k = atoi(argv[3]);
    int n_trees = atoi(argv[4]);
    int depth = atoi(argv[5]);
    int dim = atoi(argv[6]);
    int mmap = atoi(argv[7]);
    char *result_path = argv[8];
    std::string outfile_path(argv[9]);

    int last_arg = 9;
    int n_points = n - n_test;
    bool verbose = false;


    /////////////////////////////////////////////////////////////////////////////////////////
    // test mrpt
    float *train, *test;

    test = read_memory((outfile_path + "_test.bin").c_str(), n_test, dim);
    if(!test) {
        std::cerr << "Test data " << outfile_path + "_test.bin" << " could not be read\n";
        return -1;
    }

    if(mmap) {
        train = read_mmap((outfile_path + "_train.bin").c_str(), n_points, dim);
    } else {
        train = read_memory((outfile_path + "_train.bin").c_str(), n_points, dim);
    }

    if(!test) {
        std::cerr << "Training data " << outfile_path + "_train.bin" << " could not be read\n";
        return -1;
    }


    const Map<const MatrixXf> *M = new Map<const MatrixXf>(train, dim, n_points);
    float sparsity = sqrt(dim);

    Mrpt index_dense(M, n_trees, depth, sparsity);
    index_dense.grow();

    for (int arg = last_arg + 1; arg < argc; ++arg) {
        int votes = atoi(argv[arg]);
        if (votes > n_trees) continue;

        std::vector<double> times;
        std::vector<std::set<int>> idx;

        for (int i = 0; i < n_test; ++i) {
                std::vector<int> result(k);
                double start = omp_get_wtime();
                index_dense.query(Map<VectorXf>(&test[i * dim], dim), k, votes, &result[0]);

                double end = omp_get_wtime();
                times.push_back(end - start);
                idx.push_back(std::set<int>(result.begin(), result.end()));
            }

        if(verbose)
            std::cout << "k: " << k << ", # of trees: " << n_trees << ", depth: " << depth << ", sparsity: " << sparsity << ", votes: " << votes << "\n";
        else
            std::cout << k << " " << n_trees << " " << depth << " " << sparsity << " " << votes << " ";

        results(k, times, idx, (std::string(result_path) + "/truth_" + std::to_string(k)).c_str(), verbose);

    }

    delete[] test;
    if(!mmap) delete[] train;


    return 0;
}