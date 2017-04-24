#!/bin/bash
#SBATCH --time=01:00:00
#SBATCH --mem=30G
#SBATCH --cpus-per-task=1

#SBATCH -e errors_writer.txt
#SBATCH --mail-type=END
#SBATCH --mail-user=ville.o.hyvonen@helsinki.fi

BASE_DIR="$WRKDIR/Sanger/genome_test"  # set to the path of the repo

if [ "$#" -ne "3" ]; then
   echo "error: Expecting parameters: <input file> <n_train> <n_test>"
   echo "error: Expecting parameters: <input file> <n_train> <n_test>" > /dev/stderr
   exit 1
fi


# move data and binary_writer into the local disc of the node
mkdir -p "/tmp/$SLURM_JOB_ID"
cp -a "$BASE_DIR/data/$1/$1.mat" "$BASE_DIR/binary_writer/binary_writer" "/tmp/$SLURM_JOB_ID/"
cd "/tmp/$SLURM_JOB_ID"

srun ./binary_writer "$1" "$1.mat" "/tmp/$SLURM_JOB_ID/" $2 $3 > dimensions.sh

cp train.bin test.bin dimensions.sh "$BASE_DIR/data/$1/"
