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

#include "../common.h"

using namespace Eigen;


int main(int argc, char **argv) {
    size_t n_args = 4;
    if (argc != n_args + 1) {
        std::cerr << "Usage: " << argv[0] << " data_path n_points n_test dim" << std::endl;
        return 1;
    }

    std::string infile_path(argv[1]);
    if (!infile_path.empty() && infile_path.back() != '/')
      infile_path += '/';

    size_t n_points = atoi(argv[2]);
    size_t n_test = atoi(argv[3]);
    size_t dim = atoi(argv[4]);

    size_t n_train = n_points - n_test;

    float *train, *test;

    train = read_memory((infile_path + "train.bin").c_str(), n_train, dim);
    if(!train) {
        std::cerr << "in mrpt_comparison: training data " << infile_path + "train.bin" << " could not be read\n";
        return 1;
    }

    const Map<const MatrixXf> train_data(train, dim, n_train);

    test = read_memory((infile_path + "test.bin").c_str(), n_test, dim);
    if(!test) {
        std::cerr << "in mrpt_comparison: test data " << infile_path + "test.bin" << " could not be read\n";
        return 1;
    }

    const Map<const MatrixXf> test_data(test, dim, n_test);

    std::cout << "\n";
    std::cout << "training data: n = " << train_data.cols() << ", dim = " << train_data.rows() << '\n';
    std::cout << "test data: n = " << test_data.cols() << ", dim = " << test_data.rows() << "\n\n";

    std::cout << "training data:\n";
    std::cout << train_data.block(0,0,15,30) << "\n\n";
    std::cout << "test data:\n";
    std::cout << test_data.block(0,0,15,5) << "\n\n";

    return 0;
}
