#!/bin/bash
#SBATCH --time=01:00:00
#SBATCH --mem=5G
#SBATCH --cpus-per-task=1

#SBATCH -e "../slurm_output/errors_writer_$SLURM_JOB_ID.txt"
#SBATCH --
