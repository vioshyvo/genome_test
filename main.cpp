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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cpp/Mrpt.h" // sparse version on of Mrpt
#include "common.h"

using namespace Eigen;


int main(int argc, char **argv) {
    char *data_path = argv[1];
    char *data_name = argv[2];
    int n = atoi(argv[3]);
    int n_test = atoi(argv[4]);
    int k = atoi(argv[5]);

    int n_trees = atoi(argv[6]);
    int depth = atoi(argv[7]);

    char *result_path = argv[8];

    int last_arg = 8;
    int n_points = n - n_test;
    bool verbose = false;

    // std::ifstream infile("/home/hyvi/HYVI/programs/fsm-lite-master/Ecol_short100_21_41");
    // std::ifstream infile("/home/hyvi/HYVI/programs/fsm-lite-master/Ecol_short100_short");
    std::ifstream infile(data_path);

    int kmer_count = 0;

    while (infile)
        {
            // read stuff from the file into a string and print it
            std::string strInput;
            infile >> strInput;

            if(strInput.find('|') != std::string::npos) kmer_count++;

        }

    // std::cout << kmer_count << " different kmers.\n\n";

    int dim = kmer_count;

    // row = variable (k-mer)
    // col = data point (bacterial sample)
    Eigen::SparseMatrix<float> X(kmer_count, n);

    infile.clear();
    infile.seekg(0, std::ios::beg);

    BenchTimer etr;
    etr.start();

    int kmer = -1;

    typedef Eigen::Triplet<float> T;
    std::vector<T> tripletList;
    // std::cout << "max_size: " << tripletList.max_size() << "\n";

    // std::cout << "memory required: " << 4.0 * n * kmer_count / 2 / 1000000000 << " GB\n\n";

    tripletList.reserve(n * kmer_count / 2);

//    std::cout << "size: " << tripletList.size() << "\n";
//    std::cout << "capacity: " << tripletList.capacity() << "\n";
//    std::cout << "n * kmer_count: " << n * kmer_count << "\n\n";

    while(infile) {
        std::string strInput;
        infile >> strInput;
        if(std::isupper(strInput[0])) {
            kmer++;
        } else if(strInput[0] == 'f') {
            int colon_position = strInput.find(':');
            int observation = std::stoi(strInput.substr(1, colon_position - 1)) - 1;
            float value = std::stof(strInput.substr(colon_position + 1));
            tripletList.push_back(T(kmer, observation, value));
        }

    }

    X.setFromTriplets(tripletList.begin(), tripletList.end());
    X.makeCompressed();


    etr.stop();
    std::cout << "Eigen time to read the file: " << etr.value() << " seconds.\n\n";
    etr.reset();

    // std::cout << X.block(0,0,20,20) << std::endl << std::endl;
    // std::cout << "Rivejä: " << X.rows() << ", Sarakkeita: " << X.cols() << "\n\n";


    // read test points into a dense matrix
    MatrixXf X_test = MatrixXf::Zero(dim, n_test);

    for (int i = n_points; i < X.outerSize(); ++i)
        for (SparseMatrix<float>::InnerIterator it(X,i); it; ++it)
            X_test(it.row(), i - n_points) = it.value();

//    std::cout << "Rivejä: " << X_test.rows() << ", sarakkeita: " << X_test.cols() << "\n\n";
//    std::cout << X.block(0,95,20,5) << std::endl << std::endl;
//    std::cout << X_test.block(0,0,20,5) << std::endl << std::endl;


    // copy data buffer into X_test_ptr
//    float *X_test_ptr;
//    Map<MatrixXf>(X_test_ptr, X_test.rows(), X_test.cols()) = X_test;

    float *X_test_ptr = &X_test(0,0);            // get the address storing the data for m2
    // Map<MatrixXf> X_test_map(X_test_ptr, X_test.rows(), X_test.cols());   // m2map shares data with m2

    const SparseMatrix<float> *Xtrain = new SparseMatrix<float>(X.leftCols(n_points));

//    for(int j = 0; j < n_test; j++) {
//
//        Timer tmr2;
//
//        VectorXf dist(n_points);
//        for(int i = 0; i < n_points; i++)
//            dist[i] = (X.col(i) - X_test.col(j)).squaredNorm();
//
//        VectorXi idx(n_points);
//        std::iota(idx.data(), idx.data() + n_points, 0);
//        std::nth_element(idx.data(), idx.data() + k, idx.data() + n_points,
//                         [&dist](int i1, int i2) {return dist(i1) < dist(i2);});
//        std::sort(dist.data(), dist.data() + dist.size());

        // std::cout << "Ten nearest points: ";
//        for(int i = 0; i < k; i++)
//        std::cout << idx(i) << ", ";
//        std::cout << std::endl;

        // std::cout << "Ten nearest squared distances: ";
//        for(int i = 0; i < k; i++)
//            std::cout << dist[i] << ", ";

 //       t = tmr2.elapsed();
        // std::cout << "\nTime for query for one query point: " << t << " seconds.\n"<< std::endl;
    //}


    // std::cout << "proportion of non-zeros: " << X.nonZeros() / (1.0 * kmer_count * n) << "\n\n";


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// test Sanger data

    // std::string sanger_path = "/home/hyvi/HYVI/git/Sanger/results/sanger/";

    // set the parameters
    // int n_trees = 20;
    // int depth = 4;

    float sparsity = 1 / sqrt(dim);
    // std::vector<int> vote = {1,2,3,4,5};


    Mrpt index(Xtrain, n_trees, depth, sparsity);
    index.grow();

    // std::cout << "Index grown\n";


    // for(int j = 0; j < vote.size(); j++) {
    for (int arg = last_arg + 1; arg < argc; ++arg) {
        int votes = atoi(argv[arg]);

        // int votes = vote[j];
        if (votes > n_trees) continue;

        std::vector<double> times;
        std::vector<std::set<int>> idx;

        for (int i = 0; i < n_test; ++i) {
                std::vector<int> result(k);
                double start = omp_get_wtime();
                int nn = index.query(Map<VectorXf>(&X_test_ptr[i * dim], dim), k, votes, &result[0]); // <- index.query

                double end = omp_get_wtime();


                times.push_back(end - start);
                idx.push_back(std::set<int>(result.begin(), result.begin() + nn));
            }

        if(verbose)
            std::cout << "k: " << k << ", # of trees: " << n_trees << ", depth: " << depth << ", sparsity: " << sparsity << ", votes: " << votes << "\n";
        else
            std::cout << k << " " << n_trees << " " << depth << " " << sparsity << " " << votes << " ";
        results(k, times, idx, (std::string(result_path) + "/truth_" + std::to_string(k)).c_str(), verbose);

    }



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// read mnist data & set the parameters


//    int n_test;
//    int n_points;
//    int dim = 784;
//    int n_trees = 256;
//    int depth = 10;
//    int k = 10;
//    bool verbose = true;
//
//    float sparsity = 0.035;
//    std::vector<int> vote = {1,2,3,4,5,10};


//    std::string path = "/home/hyvi/HYVI/git/MRPT_test/data/mnist";
//    float *train_data = get_data((path + "/train.bin").c_str(), dim, &n_points);
//    float *test_data = get_data((path + "/test.bin").c_str(), dim, &n_test);
//
//
////    std::cout << "n_test: " << n_test << "\n";
////    std::cout << "n_train: " << n << "\n";
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// test dense version with mnist
//
//
//    const Map<const MatrixXf> *M = new Map<const MatrixXf>(train_data, dim, n_points);
//
//    std::cout << "Data read.\n";
//    std::cout << "Rivejä: " << M->rows() << "\nSarakkeita: " << M->cols() << "\n";
//    // std::cout << M->block(200,200,20,20) << std::endl << std::endl;
//
//
//
//
//    Mrpt index_dense(M, n_trees, depth, sparsity);
//    index_dense.grow();
//
//    std::cout << "Index grown\n";
//
//
//
//    for(int j = 0; j < vote.size(); j++) {
//        int votes = vote[j];
//        if (votes > n_trees) continue;
//
//        std::vector<double> times;
//        std::vector<std::set<int>> idx;
//
//        for (int i = 0; i < n_test; ++i) {
//                std::vector<int> result(k);
//                double start = omp_get_wtime();
//                index_dense.query(Map<VectorXf>(&test_data[i * dim], dim), k, votes, &result[0]);
//
//                double end = omp_get_wtime();
//                times.push_back(end - start);
//                idx.push_back(std::set<int>(result.begin(), result.end()));
//            }
//
//        if(verbose)
//            std::cout << "k: " << k << ", # of trees: " << n_trees << ", depth: " << depth << ", sparsity: " << sparsity << ", votes: " << votes << "\n";
//        else
//            std::cout << k << " " << n_trees << " " << depth << " " << sparsity << " " << votes << "\n";
//        results(k, times, idx, (std::string(path) + "/truth_" + std::to_string(k)).c_str(), verbose);
//
//    }
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// test sparse version with mnist
//
//    // read training data into a sparse matrix
//    Eigen::SparseMatrix<float> *X_train = new Eigen::SparseMatrix<float>(dim, n_points);
//
//    typedef Eigen::Triplet<float> t;
//    std::vector<t> tripletList2;
//
//    tripletList2.reserve(n_points * dim / 2);
//
//    for(int col = 0; col < n_points; ++col) {
//        for(int row = 0; row < dim; ++row) {
//            float value = train_data[col * dim + row];
//            if(value) tripletList2.push_back(t(row, col, value));
//        }
//    }
//
//    X_train->setFromTriplets(tripletList2.begin(), tripletList2.end());
//    X_train->makeCompressed();
//
//    // read test data into a sparse matrix
//    Eigen::SparseMatrix<float> *X_test = new Eigen::SparseMatrix<float>(dim, n_test);
//
//    std::vector<t> tripletList3;
//
//    tripletList3.reserve(n_test * dim / 2);
//
//    for(int col = 0; col < n_test; ++col) {
//        for(int row = 0; row < dim; ++row) {
//            float value = test_data[col * dim + row];
//            if(value) tripletList3.push_back(t(row, col, value));
//        }
//    }
//
//    X_test->setFromTriplets(tripletList3.begin(), tripletList3.end());
//    X_test->makeCompressed();
//
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
//// index grown with sparse data, queries done with dense query points
//
//    Mrpt index(X_train, n_trees, depth, sparsity);
//    index.grow();
//
//    std::cout << "Index grown\n";
//
//
//    for(int j = 0; j < vote.size(); j++) {
//        int votes = vote[j];
//        if (votes > n_trees) continue;
//
//        std::vector<double> times;
//        std::vector<std::set<int>> idx;
//
//        for (int i = 0; i < n_test; ++i) {
//                std::vector<int> result(k);
//                double start = omp_get_wtime();
//                int nn = index.query(Map<VectorXf>(&test_data[i * dim], dim), k, votes, &result[0]); // <- index.query
//
//                double end = omp_get_wtime();
//
//
//                times.push_back(end - start);
//                idx.push_back(std::set<int>(result.begin(), result.begin() + nn));
//            }
//
//        if(verbose)
//            std::cout << "k: " << k << ", # of trees: " << n_trees << ", depth: " << depth << ", sparsity: " << sparsity << ", votes: " << votes << "\n";
//        else
//            std::cout << k << " " << n_trees << " " << depth << " " << sparsity << " " << votes << "\n";
//        results(k, times, idx, (std::string(path) + "/truth_" + std::to_string(k)).c_str(), verbose);
//
//    }
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
//// index grown with sparse data, queries done with sparse query points
//    for(int j = 0; j < vote.size(); j++) {
//        int votes2 = vote[j];
//        if (votes2 > n_trees) continue;
//
//        std::vector<double> times2;
//        std::vector<std::set<int>> idx2;
//
//        for (int i = 0; i < n_test; ++i) {
//                std::vector<int> result2(k);
//                double start2 = omp_get_wtime();
//                int nn = index.sparse_query(X_test->col(i), k, votes2, &result2[0]);    // <- index.sparse_query
//
//                double end2 = omp_get_wtime();
//                times2.push_back(end2 - start2);
//                idx2.push_back(std::set<int>(result2.begin(), result2.begin() + nn));
//            }
//
//        if(verbose)
//            std::cout << "k: " << k << ", # of trees: " << n_trees << ", depth: " << depth << ", sparsity: " << sparsity << ", votes: " << votes2 << "\n";
//        else
//            std::cout << k << " " << n_trees << " " << depth << " " << sparsity << " " << votes2 << "\n";
//        results(k, times2, idx2, (std::string(path) + "/truth_" + std::to_string(k)).c_str(), verbose);
//
//    }






    return 0;
}
