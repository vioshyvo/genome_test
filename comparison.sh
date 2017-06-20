#!/bin/bash

if [ "$#" -ne 2 ] && [ "$#" -ne 3 ]; then
   echo "error: Expecting parameters: <data-set-name> <n> or <data-set-name> <n> <postfix>" 1>&2
   exit
fi

DATA_NAME="$1$2"
DATA_DIR="data/$DATA_NAME"

if [ ! -f "data/$DATA_NAME/dimensions.sh" ]; then
    echo Invalid data set $DATA_NAME 1>&2
    exit
fi

if [ "$#" -eq 2 ]; then
  PARAMETER_NAME="$DATA_NAME"
fi

if [ "$#" -eq 3 ]; then
  PARAMETER_NAME="$DATA_NAME-$3"
fi

if [ ! -f "parameters/$PARAMETER_NAME.sh" ]; then
    echo Invalid parameter file name $PARAMETER_NAME 1>&2
    exit
fi

pushd exact
make clean
make
popd

pushd mrpt
make clean
make
popd


. "parameters/$PARAMETER_NAME.sh"
. "data/$DATA_NAME/dimensions.sh"

ADD=""
if [[ $MMAP -eq 1 ]]
then
  ADD="-mmap"
  echo "memory mapping on"
fi

RESULT_DIR="results/$PARAMETER_NAME$ADD"

mkdir -p "$RESULT_DIR"
for K in 10; do
    ./exact/tester $N $N_TEST $K $DIM $MMAP "$DATA_DIR" > "$RESULT_DIR/truth_$K"
done

echo -n > "$RESULT_DIR/mrpt.txt"
for n_trees in $MRPT_VOTING_N_TREES; do
    for depth in $MRPT_DEPTH; do
        mrpt/mrpt_comparison $N $N_TEST $K $n_trees $depth $DIM $MMAP "$RESULT_DIR" "$DATA_DIR" "$MRPT_SPARSITY" $MRPT_VOTES  >> "$RESULT_DIR/mrpt.txt"
    done
done
