## Pipeline
![](Lentua.jpg)


rm_names.sh `<path-to-data> `
  - removes spades.fa - files, and renames rest fasta-files into f0001, f0002,...
  - outputs the names of the original files as file_list into the directory where called.

list_files.sh `<path-to-data>` `<name-of-output-list>`
  - makes a list of files with (identifier filename) - tuples for the fsm-lite input  

fsm_ukko.sh `<list-of-files>` `<name-of-output-file>`
  - uses fsm-lite to make read kmers counts into the sparse matrix from the specified fasta-files

binary_writer `<path-to-data>` `<name-of-output-file>` `<n_train>` `<n_test>`
  - divides the data set into a training set with n_train points and test set with n_test points and writes these to the specified directory
