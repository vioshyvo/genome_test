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

#include "cpp/Mrpt.h"
#include "common.h"

using namespace Eigen;

int read_memory(const char *file, int n_train, int n_test, int dim, float *train, float *test) {
//    train = new float[n_train * dim];
//    test = new float[n_test * dim];

    struct stat sb;
    stat(file, &sb);

    if(sb.st_size != (n_test + n_train) * dim * sizeof(float)) {
        std::cerr << "Size of the file is " << sb.st_size << ", while the expected size is: " << (n_test + n_train) * dim * sizeof(float) << "\n";
        return -1;
    }

    FILE *fd;
    if ((fd = fopen(file, "rb")) == NULL)
        return -1;

    int read = fread(train, sizeof(float), n_train * dim, fd);
    if (read != n_train * dim)
        return -1;

    read = fread(test, sizeof(float), n_test * dim, fd);
    if (read != n_test * dim)
        return -1;

    fclose(fd);
    return 0;
}


int write_binary(char *data_path, std::string outfile_path, int n_train, int n_test, int *dim) {
    std::ifstream infile(data_path);
    int n = n_train + n_test;

    if(!infile) {
        std::cerr << "Error: could not open file " << data_path << "\n";
        return -1;
    }

    FILE *outfile;
    outfile = fopen((outfile_path + "_rowwise.bin").c_str(), "wb");

    if(!outfile) {
        std::cerr << "Error: could not open output file " << outfile_path << "_rowwise.bin" << "\n";
        return -1;
    }

    // count number of kmers / dimensions of the data
    int kmer_count = 0;

    while (infile)
        {
            std::string strInput;
            infile >> strInput;

            if(strInput.find('|') != std::string::npos) kmer_count++;
        }

    infile.clear();
    infile.seekg(0, std::ios::beg);
    *dim = kmer_count;

    // read the data into a binary file in a rowwise form
    BenchTimer etr;
    etr.start();

    int kmer = -1;
    float *kmer_buffer = nullptr;

    while(infile) {
        std::string strInput;
        infile >> strInput;
        if(std::isupper(strInput[0])) {
            if(kmer_buffer) {
                fwrite(kmer_buffer, sizeof(float), n, outfile);
                delete[] kmer_buffer;
                kmer_buffer = nullptr;
            }
            kmer_buffer = new float[n]();
            kmer++;
        } else if(strInput[0] == 'f') {
            int colon_position = strInput.find(':');
            int observation = std::stoi(strInput.substr(1, colon_position - 1)) - 1;
            float value = std::stof(strInput.substr(colon_position + 1));
            kmer_buffer[observation] = value;
        }

    }

    fwrite(kmer_buffer, sizeof(float), n, outfile);
    fclose(outfile);
    delete[] kmer_buffer;
    kmer_buffer = nullptr;

    etr.stop();
    std::cout << "Time to read the original file: " << etr.value() << " seconds.\n";
    etr.reset();

    etr.start();

    // read the data from the rowwise binary file into the colwise binary file
    // reopen the rowwise binary file
    FILE *inf;
    inf = fopen((outfile_path + "_rowwise.bin").c_str(), "rb");

     // obtain file size:
    fseek (inf, 0, SEEK_END);
    size_t inf_size = ftell(inf);
    rewind (inf);

    struct stat sb;
    stat((outfile_path + "_rowwise.bin").c_str(), &sb);

    if(sb.st_size != n * kmer_count * sizeof(float)) {
        std::cerr << "Error: size of the input data is " << inf_size << ", while the expected size is " << n * kmer_count * sizeof(float) << "\n";
        return -1;
    }

    // open file for writing the matrix into colwise form
    FILE *outfile_train;
    outfile_train = fopen((outfile_path + "_train.bin").c_str(), "wb");

    if(!outfile_train) {
        std::cerr << "Error: could not open output file " << outfile_path << "_train.bin" << "\n";
        return -1;
    }

    FILE *outfile_test;
    outfile_test = fopen((outfile_path + "_test.bin").c_str(), "wb");

    if(!outfile_test) {
        std::cerr << "Error: could not open output file " << outfile_path << "_test.bin" << "\n";
        return -1;
    }

    // write the data in colwise format
    float *obs_buffer;

    for(int i = 0; i < n_train; i++) {
        obs_buffer = new float[kmer_count]();
        for(int j = 0; j < kmer_count; j++) {
            fseek(inf, (j * n + i) * sizeof(float), SEEK_SET);
            fread(obs_buffer + j, sizeof(float), 1, inf);
        }
        fwrite(obs_buffer, sizeof(float), kmer_count, outfile_train);
        delete[] obs_buffer;
        obs_buffer = nullptr;
    }

    fclose(outfile_train);

    etr.stop();
    std::cout << "Time to write the training data with " << n_train << " points: " << etr.value() << " seconds.\n";
    etr.reset();

    etr.start();


    for(int i = n_train; i < n; i++) {
        obs_buffer = new float[kmer_count]();
        for(int j = 0; j < kmer_count; j++) {
            fseek(inf, (j * n + i) * sizeof(float), SEEK_SET);
            fread(obs_buffer + j, sizeof(float), 1, inf);
        }
        fwrite(obs_buffer, sizeof(float), kmer_count, outfile_test);
        delete[] obs_buffer;
        obs_buffer = nullptr;
    }

    fclose(outfile_test);

    etr.stop();
    std::cout << "Time to write the test data with " << n_test << " points: " << etr.value() << " seconds.\n";
    etr.reset();

    return 0;
}

int read_into_sparse(Eigen::SparseMatrix<float> &X, char *data_path, int n, int *dim) {
    std::ifstream infile(data_path);

    if(!infile) {
        std::cerr << "Error: could not open file " << data_path << "\n";
        return -1;
    }

    int kmer_count = 0;
    while (infile)
        {
            std::string strInput;
            infile >> strInput;

            if(strInput.find('|') != std::string::npos) kmer_count++;

        }

    *dim = kmer_count;
    infile.clear();
    infile.seekg(0, std::ios::beg);

    // row = variable (k-mer)
    // col = data point (bacterial sample)
    X = Eigen::SparseMatrix<float>(kmer_count, n);

    BenchTimer etr;
    etr.start();

    int kmer = -1;

    typedef Eigen::Triplet<float> T;
    std::vector<T> tripletList;

    tripletList.reserve(n * kmer_count / 2);

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
    std::cout << "Time to read the file into a sparse matrix: " << etr.value() << " seconds.\n";
    etr.reset();

    return 0;
}




int main(int argc, char **argv) {
    char *data_path = argv[1];
    char *data_name = argv[2];
    int n = atoi(argv[3]);
    int n_test = atoi(argv[4]);
    int k = atoi(argv[5]);
    int n_trees = atoi(argv[6]);
    int depth = atoi(argv[7]);
    int dim = atoi(argv[8]);
    int mmap = atoi(argv[9]);
    char *result_path = argv[10];

    int last_arg = 10;
    int n_points = n - n_test;
    bool verbose = false;

    std::string outfile_path = "/home/hyvi/HYVI/data/Sanger/Ecoli/Ecoli100_short/Ecoli100_short";

    // int dim = 14541768;
    // int dim = 573;

    write_binary(data_path, outfile_path, n_points, n_test, &dim);

    Eigen::SparseMatrix<float> X;
    read_into_sparse(X, data_path, n, &dim);

    std::cout << std::endl << X.block(0,0,20,20) << std::endl << std::endl;
    std::cout << "Alkuperäinen matriisi, rivejä: " << X.rows() << ", Sarakkeita: " << X.cols() << "\n\n";

    //  read the colwise matrix from the binary file
    BenchTimer etr;
    double start = omp_get_wtime();
    etr.start();

    float *train_data_rowwise = get_data((outfile_path + "_rowwise.bin").c_str(), dim, &n_points);
    const Map<const Matrix<float, Dynamic, Dynamic,RowMajor>> *M = new Map<const Matrix<float, Dynamic, Dynamic,RowMajor>>(train_data_rowwise, dim, n_points);
    std::cout << M->block(0,0,20,20) << std::endl << std::endl;
    std::cout << "Riveittäisestä versiosta luettu matriisi, rivejä: " << M->rows() << ", Sarakkeita: " << M->cols() << "\n\n";
    delete[] train_data_rowwise;
    delete M;

    etr.stop();
    double finish = omp_get_wtime();
    std::cout << "Eigen time to read the binary: " << etr.value() << " seconds.\n\n";
    std::cout << "OMP time to read the binary: " << finish - start << " seconds.\n\n";
    etr.reset();


    float *train_data = get_data((outfile_path + "_train.bin").c_str(), dim, &n_points);
    const Map<const MatrixXf> *M1 = new Map<const MatrixXf>(train_data, dim, n_points);
    std::cout << M1->block(0,0,20,20) << std::endl << std::endl;
    std::cout << "Harjoitusaineisto, rivejä: " << M1->rows() << ", Sarakkeita: " << M1->cols() << "\n\n";
    delete[] train_data;
    delete M1;

    float *test_data = get_data((outfile_path + "_test.bin").c_str(), dim, &n_test);
    const Map<const MatrixXf> *M2 = new Map<const MatrixXf>(test_data, dim, n_test);
    std::cout << M1->block(0,0,20,5) << std::endl << std::endl;
    std::cout << "Testiaineisto, rivejä: " << M2->rows() << ", Sarakkeita: " << M2->cols() << "\n\n";
    delete[] test_data;
    delete M2;

    std::cout << X.block(0,95,20,5) << std::endl << std::endl;
    std::cout << "Alkuperäinen matriisi, rivejä: " << X.rows() << ", Sarakkeita: " << X.cols() << "\n\n";



//    /////////////////////////////////////////////////////////////////////////////////////////
//    // test mrpt
//    float *train;
//    float *test;
//
//    if(mmap) {
//    } else {
//        train = new float[n_points * dim];
//        test = new float[n_test * dim];
//        read_memory((outfile_path + ".bin").c_str(), n_points, n_test, dim, train, test);
//    }
//
//    const Map<const MatrixXf> *M = new Map<const MatrixXf>(train, dim, n_points);
//    float sparsity = sqrt(dim);
//
//    Mrpt index_dense(M, n_trees, depth, sparsity);
//    index_dense.grow();
//
//    for (int arg = last_arg + 1; arg < argc; ++arg) {
//        int votes = atoi(argv[arg]);
//        if (votes > n_trees) continue;
//
//        std::vector<double> times;
//        std::vector<std::set<int>> idx;
//
//        for (int i = 0; i < n_test; ++i) {
//                std::vector<int> result(k);
//                double start = omp_get_wtime();
//                index_dense.query(Map<VectorXf>(&test[i * dim], dim), k, votes, &result[0]);
//
//                double end = omp_get_wtime();
//                times.push_back(end - start);
//                idx.push_back(std::set<int>(result.begin(), result.end()));
//            }
//
//        if(verbose)
//            std::cout << "k: " << k << ", # of trees: " << n_trees << ", depth: " << depth << ", sparsity: " << sparsity << ", votes: " << votes << "\n";
//        else
//            std::cout << k << " " << n_trees << " " << depth << " " << sparsity << " " << votes << " ";
//
//        results(k, times, idx, (std::string(result_path) + "/truth_" + std::to_string(k)).c_str(), verbose);
//
//    }
//
//    delete[] test;
//    delete[] train;






///////////////////////////////////////////////////////////////////////7
//
//
//    // read test points into a dense matrix
//    MatrixXf X_test = MatrixXf::Zero(dim, n_test);
//
//    for (int i = n_points; i < X.outerSize(); ++i)
//        for (SparseMatrix<float>::InnerIterator it(X,i); it; ++it)
//            X_test(it.row(), i - n_points) = it.value();
//
////    std::cout << "Rivejä: " << X_test.rows() << ", sarakkeita: " << X_test.cols() << "\n\n";
////    std::cout << X.block(0,95,20,5) << std::endl << std::endl;
////    std::cout << X_test.block(0,0,20,5) << std::endl << std::endl;
//
//
//    // copy data buffer into X_test_ptr
////    float *X_test_ptr;
////    Map<MatrixXf>(X_test_ptr, X_test.rows(), X_test.cols()) = X_test;
//
//    float *X_test_ptr = &X_test(0,0);            // get the address storing the data for m2
//    // Map<MatrixXf> X_test_map(X_test_ptr, X_test.rows(), X_test.cols());   // m2map shares data with m2
//
//    const SparseMatrix<float> *Xtrain = new SparseMatrix<float>(X.leftCols(n_points));
//
////    for(int j = 0; j < n_test; j++) {
////
////        Timer tmr2;
////
////        VectorXf dist(n_points);
////        for(int i = 0; i < n_points; i++)
////            dist[i] = (X.col(i) - X_test.col(j)).squaredNorm();
////
////        VectorXi idx(n_points);
////        std::iota(idx.data(), idx.data() + n_points, 0);
////        std::nth_element(idx.data(), idx.data() + k, idx.data() + n_points,
////                         [&dist](int i1, int i2) {return dist(i1) < dist(i2);});
////        std::sort(dist.data(), dist.data() + dist.size());
//
//        // std::cout << "Ten nearest points: ";
////        for(int i = 0; i < k; i++)
////        std::cout << idx(i) << ", ";
////        std::cout << std::endl;
//
//        // std::cout << "Ten nearest squared distances: ";
////        for(int i = 0; i < k; i++)
////            std::cout << dist[i] << ", ";
//
// //       t = tmr2.elapsed();
//        // std::cout << "\nTime for query for one query point: " << t << " seconds.\n"<< std::endl;
//    //}
//
//
//    // std::cout << "proportion of non-zeros: " << X.nonZeros() / (1.0 * kmer_count * n) << "\n\n";


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// test Sanger data
//
//    // std::string sanger_path = "/home/hyvi/HYVI/git/Sanger/results/sanger/";
//
//    // set the parameters
//    // int n_trees = 20;
//    // int depth = 4;
//
//    float sparsity = 1 / sqrt(dim);
//    // std::vector<int> vote = {1,2,3,4,5};
//
//
//    Mrpt index(Xtrain, n_trees, depth, sparsity);
//    index.grow();
//
//    // std::cout << "Index grown\n";
//
//
//    // for(int j = 0; j < vote.size(); j++) {
//    for (int arg = last_arg + 1; arg < argc; ++arg) {
//        int votes = atoi(argv[arg]);
//
//        // int votes = vote[j];
//        if (votes > n_trees) continue;
//
//        std::vector<double> times;
//        std::vector<std::set<int>> idx;
//
//        for (int i = 0; i < n_test; ++i) {
//                std::vector<int> result(k);
//                double start = omp_get_wtime();
//                int nn = index.query(Map<VectorXf>(&X_test_ptr[i * dim], dim), k, votes, &result[0]); // <- index.query
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
//            std::cout << k << " " << n_trees << " " << depth << " " << sparsity << " " << votes << " ";
//        results(k, times, idx, (std::string(result_path) + "/truth_" + std::to_string(k)).c_str(), verbose);
//
//    }
//
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// read mnist data & set the parameters
//
//
////    int n_test;
////    int n_points;
////    int dim = 784;
////    int n_trees = 256;
////    int depth = 10;
////    int k = 10;
////    bool verbose = true;
////
////    float sparsity = 0.035;
////    std::vector<int> vote = {1,2,3,4,5,10};
//
//
////    std::string path = "/home/hyvi/HYVI/git/MRPT_test/data/mnist";
////    float *train_data = get_data((path + "/train.bin").c_str(), dim, &n_points);
////    float *test_data = get_data((path + "/test.bin").c_str(), dim, &n_test);
////
////
//////    std::cout << "n_test: " << n_test << "\n";
//////    std::cout << "n_train: " << n << "\n";
////
////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////// test dense version with mnist
////
////
////    const Map<const MatrixXf> *M = new Map<const MatrixXf>(train_data, dim, n_points);
////
////    std::cout << "Data read.\n";
////    std::cout << "Rivejä: " << M->rows() << "\nSarakkeita: " << M->cols() << "\n";
////    // std::cout << M->block(200,200,20,20) << std::endl << std::endl;
////
////
////
////
////    Mrpt index_dense(M, n_trees, depth, sparsity);
////    index_dense.grow();
////
////    std::cout << "Index grown\n";
////
////
////
////    for(int j = 0; j < vote.size(); j++) {
////        int votes = vote[j];
////        if (votes > n_trees) continue;
////
////        std::vector<double> times;
////        std::vector<std::set<int>> idx;
////
////        for (int i = 0; i < n_test; ++i) {
////                std::vector<int> result(k);
////                double start = omp_get_wtime();
////                index_dense.query(Map<VectorXf>(&test_data[i * dim], dim), k, votes, &result[0]);
////
////                double end = omp_get_wtime();
////                times.push_back(end - start);
////                idx.push_back(std::set<int>(result.begin(), result.end()));
////            }
////
////        if(verbose)
////            std::cout << "k: " << k << ", # of trees: " << n_trees << ", depth: " << depth << ", sparsity: " << sparsity << ", votes: " << votes << "\n";
////        else
////            std::cout << k << " " << n_trees << " " << depth << " " << sparsity << " " << votes << "\n";
////        results(k, times, idx, (std::string(path) + "/truth_" + std::to_string(k)).c_str(), verbose);
////
////    }
////
////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////// test sparse version with mnist
////
////    // read training data into a sparse matrix
////    Eigen::SparseMatrix<float> *X_train = new Eigen::SparseMatrix<float>(dim, n_points);
////
////    typedef Eigen::Triplet<float> t;
////    std::vector<t> tripletList2;
////
////    tripletList2.reserve(n_points * dim / 2);
////
////    for(int col = 0; col < n_points; ++col) {
////        for(int row = 0; row < dim; ++row) {
////            float value = train_data[col * dim + row];
////            if(value) tripletList2.push_back(t(row, col, value));
////        }
////    }
////
////    X_train->setFromTriplets(tripletList2.begin(), tripletList2.end());
////    X_train->makeCompressed();
////
////    // read test data into a sparse matrix
////    Eigen::SparseMatrix<float> *X_test = new Eigen::SparseMatrix<float>(dim, n_test);
////
////    std::vector<t> tripletList3;
////
////    tripletList3.reserve(n_test * dim / 2);
////
////    for(int col = 0; col < n_test; ++col) {
////        for(int row = 0; row < dim; ++row) {
////            float value = test_data[col * dim + row];
////            if(value) tripletList3.push_back(t(row, col, value));
////        }
////    }
////
////    X_test->setFromTriplets(tripletList3.begin(), tripletList3.end());
////    X_test->makeCompressed();
////
////
////
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////// index grown with sparse data, queries done with dense query points
////
////    Mrpt index(X_train, n_trees, depth, sparsity);
////    index.grow();
////
////    std::cout << "Index grown\n";
////
////
////    for(int j = 0; j < vote.size(); j++) {
////        int votes = vote[j];
////        if (votes > n_trees) continue;
////
////        std::vector<double> times;
////        std::vector<std::set<int>> idx;
////
////        for (int i = 0; i < n_test; ++i) {
////                std::vector<int> result(k);
////                double start = omp_get_wtime();
////                int nn = index.query(Map<VectorXf>(&test_data[i * dim], dim), k, votes, &result[0]); // <- index.query
////
////                double end = omp_get_wtime();
////
////
////                times.push_back(end - start);
////                idx.push_back(std::set<int>(result.begin(), result.begin() + nn));
////            }
////
////        if(verbose)
////            std::cout << "k: " << k << ", # of trees: " << n_trees << ", depth: " << depth << ", sparsity: " << sparsity << ", votes: " << votes << "\n";
////        else
////            std::cout << k << " " << n_trees << " " << depth << " " << sparsity << " " << votes << "\n";
////        results(k, times, idx, (std::string(path) + "/truth_" + std::to_string(k)).c_str(), verbose);
////
////    }
////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
////// index grown with sparse data, queries done with sparse query points
////    for(int j = 0; j < vote.size(); j++) {
////        int votes2 = vote[j];
////        if (votes2 > n_trees) continue;
////
////        std::vector<double> times2;
////        std::vector<std::set<int>> idx2;
////
////        for (int i = 0; i < n_test; ++i) {
////                std::vector<int> result2(k);
////                double start2 = omp_get_wtime();
////                int nn = index.sparse_query(X_test->col(i), k, votes2, &result2[0]);    // <- index.sparse_query
////
////                double end2 = omp_get_wtime();
////                times2.push_back(end2 - start2);
////                idx2.push_back(std::set<int>(result2.begin(), result2.begin() + nn));
////            }
////
////        if(verbose)
////            std::cout << "k: " << k << ", # of trees: " << n_trees << ", depth: " << depth << ", sparsity: " << sparsity << ", votes: " << votes2 << "\n";
////        else
////            std::cout << k << " " << n_trees << " " << depth << " " << sparsity << " " << votes2 << "\n";
////        results(k, times2, idx2, (std::string(path) + "/truth_" + std::to_string(k)).c_str(), verbose);
////
////    }
//





    return 0;
}
