#!/bin/bash
#SBATCH --time=01:00:00
#SBATCH --mem=30G
#SBATCH --cpus-per-task=1

#SBATCH -e errors_comparison.txt
#SBATCH --mail-type=END
#SBATCH --mail-user=ville.o.hyvonen@helsinki.fi

BASE_DIR="/scratch/cs/icl/genome_test"  # set to the path of the repo

if [ ! -f "$BASE_DIR/data/$1/dimensions.sh" ]; then
    echo "Invalid data set $1">&2
    exit
fi

if [ ! -f "$BASE_DIR/parameters/$1.sh" ]; then
    echo "Invalid data set $1">&2
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

# create a directory for the results, if it does not yet exist
mkdir -p "$BASE_DIR/results/$DATASET_NAME$ADD"

# move data and binary_writer into the local disc of the node
mkdir -p "/tmp/$SLURM_JOB_ID"
cp -a "$BASE_DIR/data/$1/train.bin" "$BASE_DIR/data/$1/test.bin" "$BASE_DIR/exact/tester" "/tmp/$SLURM_JOB_ID/"
cd "/tmp/$SLURM_JOB_ID"

for K in 1 10 100; do
    srun ./tester $N $N_TEST 10 $DIM $MMAP "/tmp/$SLURM_JOB_ID/" > "$BASE_DIR/results/$DATASET_NAME$ADD/truth_$K"
done
