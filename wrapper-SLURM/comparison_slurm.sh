#!/bin/bash

#SBATCH --time=120:00:00
#SBATCH --mem=240G
#SBATCH --cpus-per-task=1

#SBATCH -e "errors_comparison-5depth3.txt"
#SBATCH --mail-type=END
#SBATCH --mail-user=ville.o.hyvonen@helsinki.fi

if [ "$#" -ne 2 ] && [ "$#" -ne 3 ]; then
   echo "error: Expecting parameters: <data-set-name> <n> or <data-set-name> <n> <postfix>" 1>&2
   exit
fi

BASE_DIR="$WRKDIR/Sanger/genome_test"  # set to the path of the repo

DATA_NAME="$1$2"
INPUT_DIR="$BASE_DIR/data/$DATA_NAME"
PARAMETER_DIR="$BASE_DIR/parameters"


if [ ! -f "$INPUT_DIR/dimensions.sh" ]; then
    echo Invalid data set $DATA_NAME 1>&2
    exit
fi

if [ "$#" -eq 2 ]; then
  PARAMETER_NAME="$DATA_NAME"
fi

if [ "$#" -eq 3 ]; then
  PARAMETER_NAME="$DATA_NAME-$3"
fi

if [ ! -f "$PARAMETER_DIR/$PARAMETER_NAME.sh" ]; then
    echo Invalid parameter file name $PARAMETER_NAME 1>&2
    exit
fi

. "$PARAMETER_DIR/$PARAMETER_NAME.sh"
. "$INPUT_DIR/dimensions.sh"

ADD=""
if [[ $MMAP -eq 1 ]]
then
  ADD="-mmap"
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


OUTPUT_DIR="$BASE_DIR/results/$PARAMETER_NAME$ADD"

# create a directory for the results, if it does not yet exist
mkdir -p "$OUTPUT_DIR"

# if memory mapping is on, move data and tester into the local disc of the node
if [[ $MMAP -eq 1 ]]
then
  DATA_DIR="/tmp/$SLURM_JOB_ID"
  mkdir -p "$DATA_DIR"
  cp -a "$INPUT_DIR/train.bin" "$INPUT_DIR/test.bin" "$BASE_DIR/exact" "$BASE_DIR/mrpt" "$DATA_DIR"
  cd "$DATA_DIR"
else
  DATA_DIR="$INPUT_DIR"
  cd ..
fi


K=10

# for K in 1 10 100; do
    srun exact/tester $N $N_TEST $K $DIM $MMAP "$DATA_DIR" > "$OUTPUT_DIR/truth_$K"
# done

echo -n > "$OUTPUT_DIR/mrpt.txt"
for n_trees in $MRPT_VOTING_N_TREES; do
    for depth in $MRPT_DEPTH; do
        srun mrpt/mrpt_comparison $N $N_TEST $K $n_trees $depth $DIM $MMAP "$OUTPUT_DIR" "$DATA_DIR" "$MRPT_SPARSITY" $MRPT_VOTES  >> "$OUTPUT_DIR/mrpt.txt"
    done
done
