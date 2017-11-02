#!/bin/bash
#SBATCH --time=01:00:00
#SBATCH --mem=30G
#SBATCH --cpus-per-task=1

#SBATCH -e "errors-writer-$SLURM_JOB_ID.txt"
#SBATCH --mail-type=END
#SBATCH --mail-user=ville.o.hyvonen@helsinki.fi

BASE_DIR=".."  # set to the path of the repo

if [ "$#" -ne 4 ]; then
   echo "error: Expecting parameters: <data-name> <n_train> <n_test> <counts>" 1>&2
   exit
fi

DATA_NAME="$1"
N_TRAIN="$2"
N_TEST="$3"
COUNTS="$4"

DATA_DIR="$BASE_DIR/data/$DATA_NAME"
DATA_FILE="$DATA_NAME.mat"

TMP_DIR="/tmp/$SLURM_JOB_ID"

if [ ! -f "$DATA_DIR/$DATA_FILE" ]; then
  echo File $DATA_DIR/$DATA_FILE does not exist
  exit
fi

pushd ../binary_writer
make clean
make
popd

# move data and binary_writer into the local disc of the node
mkdir -p "$TMP_DIR"
cp -a "$DATA_DIR/$DATA_FILE" "$BASE_DIR/binary_writer/binary_writer" "$TMP_DIR"

pushd "$TMP_DIR"
srun ./binary_writer "$DATA_NAME" "$DATA_FILE" "$TMP_DIR" "$N_TRAIN" "$N_TEST" "$COUNTS" > dimensions.sh
popd

cp "$TMP_DIR/train.bin" "$TMP_DIR/test.bin" "$TMP_DIR/dimensions.sh" "$DATA_DIR"
