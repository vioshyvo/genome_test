#!/bin/bash
#SBATCH --time=02:00:00
#SBATCH --mem=30G
#SBATCH --cpus-per-task=1

#SBATCH -e errors_read_kmers.txt
#SBATCH --mail-type=END
#SBATCH --mail-user=ville.o.hyvonen@helsinki.fi

FSM_PATH=$WRKDIR/Sanger/fsm-lite

if [ "$#" -ne "2" ]; then
   echo "error: Expecting parameters: <data name> <n_points>"
   echo "error: Expecting parameters: <data name> <n_points>" > /dev/stderr
   exit 1
fi

DATA_NAME="$1$2"
OUTPUT_DIR="../data/$DATA_NAME"
DATA_FILE="$DATA_NAME.mat"
TMP_DIR="/tmp/$SLURM_JOB_ID"

# move input data and fsm-lite into the local disc of the node
mkdir -p "$OUTPUT_DIR"
mkdir -p "$TMP_DIR"
cp -a "../data/$1" "$FSM_PATH/fsm-lite" "$TMP_DIR"
cd "$TMP_DIR"


for f in $1/f*
do
  id=$(basename $f)
  echo $id $(readlink -f $f)
done > "$1_list"

head -$2 "$1_list" > "$1_list$2"

srun fsm-lite -l $1_list$2 -t tmp -m 21 -M 41 -s 5 -S 95 > "$DATA_FILE"

cp "$DATA_FILE" "$1_list$2" "$OUTPUT_DIR"
