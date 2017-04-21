## Pipeline
![](Lentua.jpg)


rm_names.sh `<path-to-data> `
  - removes spades.fa - files, and renames rest fasta-files into f0001, f0002,...
  - outputs the names of the original files as file_list into the directory where called.

list_files.sh `<path-to-data>` `<name-of-output-list>`
  - makes a list of files with (identifier filename) - tuples for the fsm-lite input  

fsm_ukko.sh `<list-of-files>` `<name-of-output-file>`
  - uses fsm-lite to make read kmers counts into the sparse matrix from the specified fasta-files

writer.sh `<data-set-name>` `<n_train>` `<n_test>`
  - Divides the data set into a training set with n_train points and test set with n_test points and writes these to directory data/data-set-name/ as files train.bin and test.bin. Dimensions of the data set are written to dimensions.sh
