#!/bin/bash

if [ "$#" -ne "3" ]; then
   echo "error: Expecting parameters: <data-name> <n_train> <n_test>" 1>&2
   exit
fi

((N = $2 + $3))
DATA_NAME="$1$N"
DATA_DIR="data/$DATA_NAME"
DATA_FILE="$DATA_DIR/$DATA_NAME.mat"

echo "input data = $DATA_FILE"
echo "n_train  = $2"
echo "n_test = $3"
echo "output info = $DATA_DIR/dimensions.sh"

if [ ! -f "$DATA_FILE" ]; then
  echo Error : file "$DATA_FILE" does not exist. 1>&2
  exit
fi

pushd binary_writer
make clean
make
popd

binary_writer/binary_writer $DATA_NAME $DATA_FILE $DATA_DIR $2 $3 > "$DATA_DIR/dimensions.sh"
rm "$DATA_DIR/rowwise.bin" # remove temp files
