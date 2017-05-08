#!/bin/bash
#SBATCH --time=10:00:00
#SBATCH --mem=120G
#SBATCH --cpus-per-task=1

#SBATCH -e "errors_comparison_$SLURM_JOB_ID.txt"
#SBATCH --mail-type=END
#SBATCH --mail-user=ville.o.hyvonen@helsinki.fi

BASE_DIR="$WRKDIR/Sanger/genome_test"  # set to the path of the repo

INPUT_DIR="$BASE_DIR/data/$1"

if [ ! -f "$INPUT_DIR/dimensions.sh" ]; then
    echo "Invalid data set $1" 1>&2
    exit
fi

if [ ! -f "$BASE_DIR/parameters/$1.sh" ]; then
    echo "Invalid data set $1" 1>&2
    exit
fi

if [ "$#" -ne "1" ]; then
   echo "error: Expecting parameters: <data set name>"
   echo "error: Expecting parameters: <data set name>" > /dev/stderr
   exit 1
fi

. "$INPUT_DIR/dimensions.sh"
. "$BASE_DIR/parameters/$1.sh"


ADD=""
if [[ $MMAP -eq 1 ]]
then
  ADD="_mmap"
  echo "memory mapping on"
fi

pushd ../exact
make clean
make
popd

pushd ../mrpt
make clean
make
popd


OUTPUT_DIR="$BASE_DIR/results/$DATASET_NAME$ADD"
TMP_DIR="/tmp/$SLURM_JOB_ID"

# create a directory for the results, if it does not yet exist
mkdir -p "$OUTPUT_DIR"

# move data and tester into the local disc of the node
mkdir -p "$TMP_DIR"
cp -a "$INPUT_DIR/train.bin" "$INPUT_DIR/test.bin" "$BASE_DIR/exact/tester" "$BASE_DIR/mrpt/mrpt_comparison" "$TMP_DIR"
cd "$TMP_DIR"

for K in 1 10 100; do
    srun tester $N $N_TEST $K $DIM $MMAP "$TMP_DIR" > "$OUTPUT_DIR/truth_$K"
done


echo -n > "$OUTPUT_DIR/mrpt.txt"
for n_trees in $MRPT_VOTING_N_TREES; do
    for depth in $MRPT_DEPTH; do
        srun mrpt_comparison $N $N_TEST 10 $n_trees $depth $DIM $MMAP "$OUTPUT_DIR" "$TMP_DIR" $MRPT_VOTES  >> "$OUTPUT_DIR/mrpt.txt"
    done
done
