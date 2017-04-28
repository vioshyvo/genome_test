#!/bin/bash
#SBATCH --time=01:00:00
#SBATCH --mem=30G
#SBATCH --cpus-per-task=1

#SBATCH -e errors_comparison.txt
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

ADD=""
if [[ $MMAP -eq 1 ]]
then
  ADD="_mmap"
  echo "memory mapping on"
fi

OUTPUT_DIR="$BASE_DIR/results/$DATASET_NAME$ADD"
TMP_DIR="/tmp/$SLURM_JOB_ID"

# create a directory for the results, if it does not yet exist
mkdir -p "$OUTPUT_DIR"

# move data and tester into the local disc of the node
mkdir -p "$TMP_DIR"
cp -a "$INPUT_DIR/train.bin" "$INPUT_DIR/test.bin" "$BASE_DIR/exact/tester" "$TMP_DIR"
cd "$TMP_DIR"

for K in 1 10 100; do
    srun ./tester $N $N_TEST 10 $DIM $MMAP "$TMP_DIR" > "$OUTPUT_DIR/truth_$K"
done


echo -n > "$OUTPUT_DIR/mrpt.txt"
for n_trees in $MRPT_VOTING_N_TREES; do
    for depth in $MRPT_DEPTH; do
        mrpt/mrpt_comparison $N $N_TEST 10 $n_trees $depth $DIM $MMAP "$OUTPUT_DIR" "$TMP_DIR" $MRPT_VOTES  >> "$OUTPUT_DIR/mrpt.txt"
    done
done
