#!/bin/bash

if [ "$#" -ne 1 ] && [ "$#" -ne 2 ]; then
   echo "error: Expecting parameters: <data-set-name> OR <data-set-name> <k>" 1>&2
   exit
fi

DATA_NAME="$1"
INPUT_DIR="data/$DATA_NAME"
MMAP=0

if [ ! -f "$INPUT_DIR/dimensions.sh" ]; then
    echo Invalid data set $DATA_NAME 1>&2
    exit
fi

. "$INPUT_DIR/dimensions.sh"

if [ "$#" -eq 1 ]; then
  ((K = $N - $N_TEST))
else
  K="$2"
fi

pushd exact
make clean
make
popd

RESULT_DIR="results/$DATA_NAME-exact"

mkdir -p "$RESULT_DIR"
exact/tester "$N" "$N_TEST" "$K" "$DIM" "$MMAP" "$INPUT_DIR" > "$RESULT_DIR/truth_$K"
