#!/bin/bash

if [ "$#" -ne 2 ] && [ "$#" -ne 3 ]; then
   echo "error: Expecting parameters: <data-name> <n> or <data-name> <n> <data-monicker>" 1>&2
   exit
fi

if [ "$#" -eq 2 ]; then
  DATA_NAME="$1$2"
else
  DATA_NAME="$1$2-$3"
fi

DATA_DIR="data/$DATA_NAME"

if [ ! -f "$DATA_DIR/dimensions.sh" ]; then
    echo Invalid data set $DATA_NAME 1>&2
    exit
fi

pushd test
make clean
make
popd

. "$DATA_DIR/dimensions.sh"

echo "input data = $DATA_NAME"
echo "data directory = $DATA_DIR"
echo "n_points  = $N"
echo "n_test = $N_TEST"

 test/test_writer "$DATA_DIR" "$N" "$N_TEST" "$DIM"
