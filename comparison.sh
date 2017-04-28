#!/bin/bash

if [ ! -f "data/$1/dimensions.sh" ]; then
    echo Invalid data set 1>&2
    exit
fi

if [ ! -f "parameters/$1.sh" ]; then
    echo Invalid data set 1>&2
    exit
fi

. "parameters/$1.sh"
. "data/$1/dimensions.sh"

ADD=""
if [[ $MMAP -eq 1 ]]
then
  ADD="_mmap"
  echo "memory mapping on"
fi

mkdir -p "results/$DATASET_NAME$ADD"
for K in 1 10 100; do
    ./exact/tester $N $N_TEST $K $DIM $MMAP "data/$DATASET_NAME/" > "results/$DATASET_NAME$ADD/truth_$K"
done

echo -n > "results/$DATASET_NAME$ADD/mrpt.txt"
for n_trees in $MRPT_VOTING_N_TREES; do
    for depth in $MRPT_DEPTH; do
        mrpt/mrpt_comparison $N $N_TEST 10 $n_trees $depth $DIM $MMAP "results/$DATASET_NAME$ADD/" "data/$DATASET_NAME/" $MRPT_VOTES  >> "results/$DATASET_NAME$ADD/mrpt.txt"
    done
done
