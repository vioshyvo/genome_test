## Pipeline
![](Lentua.jpg)


`rm_names.sh <path-to-data> `
  - Removes `spades.fa` - files, and renames rest fasta-files into `f0001`, `f0002`,...
  - Outputs the names of the original files as `file_list` into the directory where called.

`read_kmers.sh <data-name> <n-points>`
  - Wrapper for `fsm-lite`, reads  kmers counts into the sparse matrix `data/<data-name><n_points>/<data-name><n_points>.mat` from the specified fasta-files, that reside in the directory `data/data-name`.
  - Argument `n_points` controls how many first points you want to read.

`binary_writer/writer.sh <data-name> <n_train> <n_test>`
  - Divides the data set into a training set with n_train points and test set with n_test points and writes these to directory `data/data-name/` as files `train.bin` and `test.bin`. Dimensions of the data set are written to `dimensions.sh`. Wrapper for `binary_writer/binary_writer`.

### SLURM wrappers

For SLURM scripts remember to set
   - upper limit for memory, for example 5 gigabytes: `#SBATCH --mem=5G`
   - upper limit for computing time, for example one hour: `#SBATCH --time=01:00:00`

Scripts that can be used to run the C++ tools in SLURM system are in the directory wrapper-SLURM:

`read_kmers_slurm.sh <data-name> <n-points>`
  - Wrapper for `fsm-lite`, same as `read_kmers.sh`.
  - Set variable `FSM_PATH` to the directory where you have compiled `fsm-lite`.


  | Data points       | run time (dd:hh:mm) | RAM    | Dimension      | Size (sparse) | Size (dense) |
  |-------------------| ------------------- | ------ | -------------- |---------------| ------------ |
  | 100               | 00:42:00            | 8.7 Gb | 14 541 768     | 3.7 Gb        | 5.5 Gb       |
  | 250               | 00:00:00            | 0.0 Gb | 16 775 795     | 3.9 Gb        | 16.6 Gb      |


  Running times and RAM consumption for `fsm-lite` in Aalto Triton cluster.

`writer_slurm.sh <data-set-name>  <n_train>  <n_test>`
 - Wrapper for `binary_writer/binary_writer`, same functionality as `writer.sh`.
 - Set variable `BASE_DIR` to your local clone of this repo, for example `BASE_DIR=/home/mydir/genome_test`
 - Remember to set
    - upper limit for memory, for example 5 gigabytes: `#SBATCH --mem=5G`
    - upper limit for computing time, for example one hour: `#SBATCH --time=01:00:00`
