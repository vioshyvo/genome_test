#!/bin/bash
#SBATCH --time=01:00:00
#SBATCH --mem=30G
#SBATCH --cpus-per-task=1

#SBATCH -e errors_writer.txt
#SBATCH --mail-type=END
#SBATCH --mail-user=ville.o.hyvonen@helsinki.fi

BASE_DIR="$WRKDIR/Sanger/genome_test"  # set to the path of the repo

if [ "$#" -ne "3" ]; then
   echo "error: Expecting parameters: <data-name> <n_train> <n_test>" 1>&2
   exit 
fi

((N = $2 + $3))
DATA_NAME="$1$N"
DATA_DIR="$BASE_DIR/data/$DATA_NAME"
DATA_FILE="$DATA_NAME.mat"

TMP_DIR="/tmp/$SLURM_JOB_ID"


if [ ! -f "$DATA_DIR/$DATA_FILE" ]; then
  echo File $DATA_DIR/$DATA_FILE does not exist
  exit
fi

# move data and binary_writer into the local disc of the node
mkdir -p "$TMP_DIR"
cp -a "$DATA_DIR/$DATA_FILE" "$BASE_DIR/binary_writer/binary_writer" "$TMP_DIR"
cd "$TMP_DIR"

srun ./binary_writer "$DATA_NAME" "$DATA_FILE" "$TMP_DIR" $2 $3 > dimensions.sh

cp train.bin test.bin dimensions.sh "$DATA_DIR"
