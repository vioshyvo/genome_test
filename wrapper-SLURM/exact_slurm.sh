#!/bin/bash

#SBATCH --time=1:00:00
#SBATCH --mem=30G
#SBATCH --cpus-per-task=1

#SBATCH -e "errors-exact.txt"
#SBATCH --mail-type=END
#SBATCH --mail-user=ville.o.hyvonen@helsinki.fi

if [ "$#" -ne 2 ]; then
   echo "error: Expecting parameters: <data-set-name> <k>" 1>&2
   exit
fi

BASE_DIR=".."  # set to the path of the repo

DATA_NAME="$1"
K="$2"
INPUT_DIR="$BASE_DIR/data/$DATA_NAME"
MMAP=0

if [ ! -f "$INPUT_DIR/dimensions.sh" ]; thene
    echo Invalid data set $DATA_NAME 1>&2
    exit
fi

. "$INPUT_DIR/dimensions.sh"

pushd ../exact
make clean
make
popd

OUTPUT_DIR="$BASE_DIR/results/$DATA_NAME-exact"

# create a directory for the results, if it does not yet exist
mkdir -p "$OUTPUT_DIR"

srun "$BASE_DIR/exact/tester" "$N" "$N_TEST" "$K" "$DIM" "$MMAP" "$INPUT_DIR" > "$OUTPUT_DIR/truth_$K"
