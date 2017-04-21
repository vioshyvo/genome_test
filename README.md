## Pipeline
![](Lentua.jpg)


`rm_names.sh <path-to-data> `
  - Removes `spades.fa` - files, and renames rest fasta-files into `f0001`, `f0002`,...
  - Outputs the names of the original files as `file_list` into the directory where called.

`list_files.sh <path-to-data> <name-of-output-list>`
  - Makes a list of files with (identifier filename) - tuples for the `fsm-lite` input.  

`fsm_ukko.sh <list-of-files> <name-of-output-file>`
  - Uses `fsm-lite` to make read kmers counts into the sparse matrix from the specified fasta-files.

`binary_writer/writer.sh <data-set-name> <n_train> <n_test>`
  - Divides the data set into a training set with n_train points and test set with n_test points and writes these to directory `data/data-set-name/` as files `train.bin` and `test.bin`. Dimensions of the data set are written to `dimensions.sh`. Wrapper for `binary_writer/binary_writer`.

### SLURM wrappers

Scripts that can be used to run the C++ tools in SLURM system are in the directory wrapper-SLURM:

writer_slurm.sh `<data-set-name>` `<n_train>` `<n_test>`
 - Wrapper for `binary_writer/binary_writer`, same functionality as `writer.sh`.
 - Set variable `BASE_DIR` to your local clone of this repo, for example `BASE_DIR=/home/mydir/genome_test`
 - Remember to set
    - upper limit for memory, for example 5 gigabytes: `#SBATCH --mem=5G`
    - upper limit for computing time, for example one hour: `#SBATCH --time=01:00:00`
