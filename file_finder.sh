#!/bin/bash

if [ "$#" -ne 2 ]; then
   echo "error: Expecting parameters: <data-set-name> <k>" 1>&2
   exit
fi

DATA_NAME="$1"
K="$2"
PERMUTATIONS_FILE="data/$DATA_NAME/permutations"
EXACT_FILE="results/$DATA_NAME-exact/truth_$K"
FILE_LIST="data/Ecol_file_list"

pushd file_finder
make
popd

file_finder/file_finder "$PERMUTATIONS_FILE" "$EXACT_FILE" "$FILE_LIST"
