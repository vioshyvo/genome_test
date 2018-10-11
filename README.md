## Pipeline
![](Lentua.jpg)

### Installing C++ tools
`./install_fsm.sh`
  - Installs [`fsm-lite`](https://github.com/nvalimak/fsm-lite "fsm-lite github page"), if not already installed.
  - Installs [`Eigen 3.3.4`](http://eigen.tuxfamily.org/index.php?title=Main_Page "eigen main page"), if not already installed.
  - Installs [`googletest`](https://github.com/google/googletest "googletest github page"), if not already installed.


### Local scripts
Scripts that can be used to run C++ tools locally

`./prepare_data.sh <zip-file> <data-name> `
  - Unzip data from `zip-file` (give full path of the zip file), preprocess it for `fsm-lite`, and save the files to `data/data-name`
  - Removes `spades.fa` - files, and renames rest fasta-files into `f0001`, `f0002`,...
  - Outputs the names of the original files as `data_name_file_list` into the `data` directory.

`./read_kmers.sh <data-name> <n-points>`
  - Wrapper for `fsm-lite`, reads  kmers counts into the sparse matrix `data/<data-name><n_points>/<data-name><n_points>.mat` from the specified fasta-files, that reside in the directory `data/data-name`.
  - Argument `data-name` is the name of the data set, for example `Ecol`.
  - Argument `n_points` controls how many first points you want to read, for example 250.
  - Creates for example file `data/Ecol250/Ecol250.mat`.

`./writer.sh <data-name> <n_train> <n_test> <counts>`
  - Divides the data set into a training set with `n_train` points and test set with `n_test` points and writes these to directory `data/data-name/` as files `train.bin` and `test.bin`. Dimensions of the data set are written to `dimensions.sh`. Wrapper for `binary_writer/binary_writer`.
  - Assumes that the data set with `n_points` points is written by `read_kmers`.
  - Argument `counts` controls if the kmer counts (`counts=1`) in samples are written, or only binary (`counts=0`) yes/no (kmer is in sample or not).

`./comparison.sh <data-name> <n> <postfix>`
  - Run exact k-NN search and approximate k-NN search with the MRPT algorithm.
  - Wrapper for `exact/tester` and `mrpt/mrpt_comparison`.
  - Assumes that the parameters of the test run are saved in the file `parameters/<data-name><n><postfix>.sh`
  or  `parameters/<data-name><n>.sh` (parameter `<postfix>` is optional).
  - If data set (such as `mnist`) name has no sample size, you can give empty string (`""`) as the second argument `n`.
  - Saves results into a directory `results/<data-name><n><postfix>` (or into `parameters/<data-name><n>`, respectively).

`./file_finder.sh <data_name> <k>`
  - write nearest neighbors of test set to file
  - `k` is number of nearest neighbors written
  - exact results should exist in `results/data-name-exact/truth_k`-file
  - results (one file for each of the point of the test set) are written into  the directory `results/data-name-exact/file_names`

### SLURM wrappers

For SLURM scripts remember to set
   - upper limit for memory, for example 5 gigabytes: `#SBATCH --mem=5G`
   - upper limit for computing time, for example one hour: `#SBATCH --time=01:00:00`

Scripts that can be used to run the C++ tools in SLURM system are in the directory wrapper-SLURM:

`prepare_data_slurm.sh <zip-file> <data-name>`
  - Slurm wrapper for `prepare_data.sh`.

`read_kmers_slurm.sh <data-name> <n-points>`
  - Wrapper for `fsm-lite`, has same arguments as `read_kmers.sh`.
  - Set variable `BASE_DIR` to your local clone of this repo, for example `BASE_DIR=/home/mydir/genome_test`


`writer_slurm.sh <data-set-name>  <n_train>  <n_test> <counts>`
 - Wrapper for `binary_writer/binary_writer`, same functionality as `writer.sh`.
 - Set variable `BASE_DIR` to your local clone of this repo, for example `BASE_DIR=/home/mydir/genome_test`
 - For `Ecol` data set with 1500 points `#SBATCH --mem=150G` and `#SBATCH --time=02:00:00` are good values.

`comparison_slurm.sh <data-name> <n> <postfix>`
  - Wrapper for `exact/tester` and `mrpt/mrpt_comparison`, same functionality as `comparison.sh`.
  - Set variable `BASE_DIR` to your local clone of this repo, for example `BASE_DIR=/home/mydir/genome_test`


### Plot results

`python plot.py <k> results/<result-name1>/mrpt.txt results/<result-name2>/mrpt.txt`
  - plots running time vs. accuracy for `k`-nn queries.
  - one line for each of the results file.
  - uses sparsity values (expected proportion of the non-zero components in the random vectors) in the legend.  
  - configuration is done straight  to the script:
    - `n_test` : test set size.
    - `legend` : draw legend or not.
    - `save` : is file saved into a file called `file_name` or showed.
    - `log` : is the scale of y-axis logarithmic or linear
    - `set_ylim` : is the limit of y axis set to `ylim`, or show all data points.
    - `legend_label` : which attribute is used for legend; current choices are `sparsity`, `depth`, and `filename`.
    - `show_title` : add title given by the argument `title` to plots.
    - `exact_time` : time of exact search for one query point.

### Misc scripts

`get_mnist.sh`
  - load mnist data set into `data/mnist/` for testing.
  - converts it into binary form (float array in saved in col-major form, dimension of data is `d = 784`).
  - loads the whole data set (`data.bin`), and divides it into a training set (`train.bin`) and a test set (`test.bin`); the test set has `TEST_N = 100` points the and training set has 59900 points with this value of `TEST_N`.
