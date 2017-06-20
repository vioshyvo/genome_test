## Pipeline
![](Lentua.jpg)

### Installing C++ tools
`./install_fsm.sh`
  - Installs [`fsm-lite`](https://github.com/nvalimak/fsm-lite "fsm-lite github page") and its dependencies ([`sdsl-lite`](https://github.com/simongog/sdsl-lite "sdsl-lite github page"), which requires [`cmake`](https://cmake.org/download/ "cmake download page")).

### Local scripts
Scripts that can be used to run C++ tools locally

`./prepare_data.sh <zip-file> <data-name> `
  - Unzip data from `zip-file`, preprocess it for `fsm-lite`, and save the files to `data/data-name`
  - Removes `spades.fa` - files, and renames rest fasta-files into `f0001`, `f0002`,...
  - Outputs the names of the original files as `data_name_file_list` into the `data` directory.

`./read_kmers.sh <data-name> <n-points>`
  - Wrapper for `fsm-lite`, reads  kmers counts into the sparse matrix `data/<data-name><n_points>/<data-name><n_points>.mat` from the specified fasta-files, that reside in the directory `data/data-name`.
  - Argument `data-name` is the name of the data set, for example `Ecol`.
  - Argument `n_points` controls how many first points you want to read, for example 250.
  - Creates for example file `data/Ecol250/Ecol250.mat`.

`./writer.sh <data-name> <n_train> <n_test>`
  - Divides the data set into a training set with n_train points and test set with n_test points and writes these to directory `data/data-name/` as files `train.bin` and `test.bin`. Dimensions of the data set are written to `dimensions.sh`. Wrapper for `binary_writer/binary_writer`.
  - Assumes that there is a data set `data-name` with `n_train + n_test` points written by `read_kmers`.

`./comparison.sh <data-name> <n> <postfix>`
  - Run exact k-NN search and approximate k-NN search with the MRPT algorithm.
  - Wrapper for `exact/tester` and `mrpt/mrpt_comparison`.
  - Assumes that the parameters of the test run are saved in the file `parameters/<data-name><n><postfix>.sh`
  or  `parameters/<data-name><n>.sh` (parameter `<postfix>` is optional).
  - Saves results into a directory `results/<data-name><n><postfix>` (or into `parameters/<data-name><n>`, respectively).

### SLURM wrappers

For SLURM scripts remember to set
   - upper limit for memory, for example 5 gigabytes: `#SBATCH --mem=5G`
   - upper limit for computing time, for example one hour: `#SBATCH --time=01:00:00`

Scripts that can be used to run the C++ tools in SLURM system are in the directory wrapper-SLURM:

`prepare_data_slurm.sh <zip-file> <data-name>`
  - Slurm wrapper for `prepare_data.sh`.

`read_kmers_slurm.sh <data-name> <n-points>`
  - Wrapper for `fsm-lite`, same as `read_kmers.sh`.
  - Set variable `FSM_PATH` to the directory where you have compiled `fsm-lite`.

`writer_slurm.sh <data-set-name>  <n_train>  <n_test>`
 - Wrapper for `binary_writer/binary_writer`, same functionality as `writer.sh`.
 - Set variable `BASE_DIR` to your local clone of this repo, for example `BASE_DIR=/home/mydir/genome_test`
 - For `Ecol` data set with 1500 points `#SBATCH --mem=150G` and `#SBATCH --time=02:00:00` are good values.

`comparison_slurm.sh <data-name> <n> <postfix>`
  - Wrapper for `exact/tester` and `mrpt/mrpt_comparison`, same functionality as `comparison.sh`.

### Plot results

`python plot.py <k> results/<result-name1>/mrpt.txt results/<result-name2>/mrpt.txt`
  - plots running time vs. accuracy for `k`-nn queries.
  - one line for each of the results file  
