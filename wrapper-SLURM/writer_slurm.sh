#!/bin/bash
if [ "$#" -ne "3" ]; then
   echo "error: Expecting parameters: <input file> <n_test> <n_train>"
   echo "error: Expecting parameters: <input file> <n_test> <n_train>" > /dev/stderr
   exit 1
fi

#SBATCH --time=01:00:00
#SBATCH --mem=5G
#SBATCH --cpus-per-task=1

#SBATCH -e "../slurm_output/errors_writer_$SLURM_JOB_ID.txt"
#SBATCH --mail-type=END
#SBATCH --mail-user=ville.o.hyvonen@helsinki.fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )" # get the directory of this script

# move data and binary_writer into the local disc of the node
mkdir /tmp/$SLURM_JOB_ID
cp -a "$DIR/../data/$1/$1.mat" "$DIR/../binary_writer/binary_writer" /tmp/$SLURM_JOB_ID/
cd /tmp/$SLURM_JOB_ID

srun binary_writer "$1" "$1.mat" "." $2 $3 > dimensions.sh

cp train.bin test.bin dimensions.sh "$DIR/../data/$1/"
