#!/bin/bash
if [ "$#" -ne "3" ]; then
   echo "error: Expecting parameters: <input file> <n_test> <n_train>"
   echo "error: Expecting parameters: <input file> <n_test> <n_train>" > /dev/stderr
   exit 1
fi

echo "input data = data/$1/$1.mat"
echo "n_train  = $2"
echo "n_test = $3"
echo "output info = data/$1/dimensions.sh"

binary_writer/binary_writer "$1" "data/$1/$1.mat" "data/$1/" $2 $3 > "data/$1/dimensions.sh"
rm "data/$1/rowwise.bin" # remove temp files
