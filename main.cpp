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
#include "common.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>



using namespace Eigen;

int read_memory(const char *file, int n, int dim, float *data) {
//    train = new float[n_train * dim];
//    test = new float[n_test * dim];

    struct stat sb;
    stat(file, &sb);

    if(sb.st_size != n * dim * sizeof(float)) {
        std::cerr << "Size of the file is " << sb.st_size << ", while the expected size is: " << n * dim * sizeof(float) << "\n";
        return -1;
    }

    FILE *fd;
    if ((fd = fopen(file, "rb")) == NULL)
        return -1;

    int read = fread(data, sizeof(float), n * dim, fd);
    if (read != n * dim)
        return -1;

    fclose(fd);
    return 0;
}

float *read_mmap(char *file, int n, int dim) {
    FILE *fd;
    if ((fd = fopen(file, "rb")) == NULL)
        return NULL;

    float *data;

    if ((data = reinterpret_cast<float *> (
#ifdef MAP_POPULATE
            mmap(0, n * dim * sizeof(float), PROT_READ,
            MAP_SHARED | MAP_POPULATE, fileno(fd), 0))) == MAP_FAILED) {
#else
            mmap(0, n * dim * sizeof(float), PROT_READ,
            MAP_SHARED, fileno(fd), 0))) == MAP_FAILED) {
#endif
            return NULL;
    }

    fclose(fd);
    return data;
}




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

    test = new float[n_test * dim];
    read_memory((outfile_path + "_test.bin").c_str(), n_test, dim, test);

    if(mmap) {
    } else {
        train = new float[n_points * dim];
        read_memory((outfile_path + "_train.bin").c_str(), n_points, dim, train);
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
    delete[] train;


    return 0;
}
