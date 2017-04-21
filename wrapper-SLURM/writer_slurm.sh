#!/bin/bash
BASE_DIR="$WRKDIR/Sanger/genome_test"  # set to the path of the repo

if [ "$#" -ne "3" ]; then
   echo "error: Expecting parameters: <input file> <n_test> <n_train>"
   echo "error: Expecting parameters: <input file> <n_test> <n_train>" > /dev/stderr
   exit 1
fi

#SBATCH --time=01:00:00
#SBATCH --mem=5G
#SBATCH --cpus-per-task=1

#SBATCH -e errors_writer_$SLURM_JOB_ID.txt
#SBATCH --mail-type=END
#SBATCH --mail-user=ville.o.hyvonen@helsinki.fi

# move data and binary_writer into the local disc of the node
mkdir -p "/tmp/$SLURM_JOB_ID/$1"
cp -a "$BASE_DIR/data/$1/$1.mat" "$BASE_DIR/binary_writer/binary_writer" "/tmp/$SLURM_JOB_ID/$1/"

srun binary_writer "$1" "/tmp/$SLURM_JOB_ID/$1/$1.mat" "/tmp/$SLURM_JOB_ID/$1/" $2 $3 > dimensions.sh

cp "/tmp/$SLURM_JOB_ID/$1/train.bin" "/tmp/$SLURM_JOB_ID/$1/test.bin" "/tmp/$SLURM_JOB_ID/$1/dimensions.sh" "$BASE_DIR/data/$1/"
