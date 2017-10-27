#include <iostream>
#include <string>
#include <Eigen/Dense>
#include <omp.h>
#include "cpp/Mrpt.h"
#include "common.h"

int main(int argc, char** argv) {
  

  std::string base_path("../..");
  std::string path(base_path + "/data/mnist");
  std::cout << base_path << std::endl;
  std::cout << path << std::endl;
  float *train_data = get_data((std::string(path) + "/train.bin").c_str(), dim, &n);
  float *test_data = get_data((std::string(path) + "/test.bin").c_str(), dim, &n_test);
  std::cout << "Moi!" << std::endl;
  return 0;
}
