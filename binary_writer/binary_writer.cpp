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

#include "../Mrpt.h"
#include "../common.h"

int main(int argc, char **argv) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " data_path outfile_path n_train n_test" << std::endl;
        return 1;
    }

    char *data_path = argv[1];
    std::string outfile_path(argv[2]);
    int n_train = atoi(argv[3]);
    int n_test = atoi(argv[4]);

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

    // count number of kmers /data
    int kmer_count = 0;

    while (infile)
        {
            std::string strInput;
            infile >> strInput;

            if(strInput.find('|') != std::string::npos) kmer_count++;
        }

    infile.clear();
    infile.seekg(0, std::ios::beg);

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
    // float *obs_buffer;
    float *obs_buffer = new float[kmer_count]();


    for(int i = 0; i < n_train; i++) {
        // obs_buffer = new float[kmer_count]();
        for(int j = 0; j < kmer_count; j++) {
            fseek(inf, (j * n + i) * sizeof(float), SEEK_SET);
            fread(obs_buffer + j, sizeof(float), 1, inf);
        }
        fwrite(obs_buffer, sizeof(float), kmer_count, outfile_train);
        memset(obs_buffer, 0, kmer_count * sizeof(float));
        //delete[] obs_buffer;
        //obs_buffer = nullptr;
    }


    fclose(outfile_train);

    etr.stop();
    std::cout << "Time to write the training data with " << n_train << " points: " << etr.value() << " seconds.\n";
    etr.reset();

    etr.start();


    for(int i = n_train; i < n; i++) {
        // obs_buffer = new float[kmer_count]();
        for(int j = 0; j < kmer_count; j++) {
            fseek(inf, (j * n + i) * sizeof(float), SEEK_SET);
            fread(obs_buffer + j, sizeof(float), 1, inf);
        }
        fwrite(obs_buffer, sizeof(float), kmer_count, outfile_test);
        memset(obs_buffer, 0, kmer_count * sizeof(float));
        //delete[] obs_buffer;
        //obs_buffer = nullptr;
    }

    delete[] obs_buffer;
    obs_buffer = nullptr;
    fclose(outfile_test);

    etr.stop();
    std::cout << "Time to write the test data with " << n_test << " points: " << etr.value() << " seconds.\n";
    etr.reset();

    std::cout << "Dimension: " << kmer_count << "\n";
    return 0;
}