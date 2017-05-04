#include <iostream>
#include <fstream>
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

#include <sys/mman.h>
#include <cstdlib>

#include <cstring>


int main(int argc, char **argv) {
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] << " data_name data_file outfile_path n_train n_test" << std::endl;
        return 1;
    }

    std::cout << "#!/bin/bash\n\n";

    std::string data_name(argv[1]);
    char *data_file = argv[2];
    std::string outfile_path(argv[3]);
    if (!outfile_path.empty() && outfile_path.back() != '/')
      outfile_path += '/';

    size_t n_train = atoi(argv[4]);
    size_t n_test = atoi(argv[5]);

    std::ifstream infile(data_file);
    size_t n = n_train + n_test;

    if(!infile) {
        std::cerr << "Error: could not open file " << data_file << "\n";
        return -1;
    }

    FILE *outfile;
    outfile = fopen((outfile_path + "rowwise.bin").c_str(), "wb");

    if(!outfile) {
        std::cerr << "Error: could not open output file " << outfile_path << "rowwise.bin" << "\n";
        return -1;
    }

    // count number of kmers /data
    size_t kmer_count = 0;

    while (infile)
        {
            std::string strInput;
            infile >> strInput;

            if(strInput.find('|') != std::string::npos) kmer_count++;
        }

    infile.clear();
    infile.seekg(0, std::ios::beg);

    // std::cout << "# Dimension of data: " << kmer_count << "\n";


    // read the data into a binary file in a rowwise form
    // double start = omp_get_wtime();

    size_t kmer = -1;
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

    // double end = omp_get_wtime();
    // std::cout << "# Time to read the original file: " << end - start << " seconds.\n";

    // read the data from the rowwise binary file into the colwise binary file
    // reopen the rowwise binary file
    FILE *inf;
    inf = fopen((outfile_path + "rowwise.bin").c_str(), "rb");

     // obtain file size:
    fseek (inf, 0, SEEK_END);
    size_t inf_size = ftell(inf);
    rewind (inf);

    struct stat sb;
    stat((outfile_path + "rowwise.bin").c_str(), &sb);

    if(sb.st_size != n * kmer_count * sizeof(float)) {
        std::cerr << "Error: size of the input data is " << inf_size << ", while the expected size is " << n * kmer_count * sizeof(float) << "\n";
        return -1;
    }

    // open file for writing the matrix into colwise form
    FILE *outfile_train;
    outfile_train = fopen((outfile_path + "train.bin").c_str(), "wb");

    if(!outfile_train) {
        std::cerr << "Error: could not open output file " << outfile_path << "train.bin" << "\n";
        return -1;
    }

    FILE *outfile_test;
    outfile_test = fopen((outfile_path + "test.bin").c_str(), "wb");

    if(!outfile_test) {
        std::cerr << "Error: could not open output file " << outfile_path << "test.bin" << "\n";
        return -1;
    }

    // write the data in colwise format
    // float *obs_buffer;
    // start = omp_get_wtime();
    float *rowwise_data = new float[kmer_count * n]();
    fread(rowwise_data, sizeof(float), n * kmer_count, inf);
    // end = omp_get_wtime();
    // std::cout << "# Time to read the rowwise data: " << end - start << " s.\n";

    // start = omp_get_wtime();
    float *obs_buffer = new float[kmer_count]();

    for(size_t i = 0; i < n_train; i++) {
      // double start_point = omp_get_wtime();
        for(size_t j = 0; j < kmer_count; j++) {
            obs_buffer[j] = rowwise_data[j * n + i];
        }
        fwrite(obs_buffer, sizeof(float), kmer_count, outfile_train);
        //memset(obs_buffer, 0, kmer_count * sizeof(float));
        // double end_point = omp_get_wtime();
        // std::cout << "# Time to write " << i + 1 << ":th data point: " << end_point - start_point << " s.\n";
    }

    fclose(outfile_train);

    // end = omp_get_wtime();
    // std::cout << "# Time to write the training data with " << n_train << " points: " << end - start << " seconds.\n";

    // start = omp_get_wtime();

    for(size_t i = n_train; i < n; i++) {
        for(size_t j = 0; j < kmer_count; j++) {
            obs_buffer[j] = rowwise_data[j * n + i];
        }
        fwrite(obs_buffer, sizeof(float), kmer_count, outfile_test);
        // memset(obs_buffer, 0, kmer_count * sizeof(float));
    }

    delete[] obs_buffer;
    obs_buffer = nullptr;
    delete[] rowwise_data;
    rowwise_data = nullptr;

    fclose(outfile_test);

    // end = omp_get_wtime();
    // std::cout << "# Time to write the test data with " << n_test << " points: " << end - start << " seconds.\n";

    std::cout << "\n";
    std::cout << "DATASET_NAME=" << data_name << "\n";
    std::cout << "N=" << n_train + n_test << "\n";
    std::cout << "N_TEST=" << n_test << "\n";
    std::cout << "DIM=" << kmer_count << "\n";

    return 0;
}
