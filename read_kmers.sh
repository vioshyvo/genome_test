#!/bin/bash
FSM_PATH=fsm-lite

if [ "$#" -ne "2" ]; then
   echo "error: Expecting parameters: <data name> <n_points>"
   echo "error: Expecting parameters: <data name> <n_points>" > /dev/stderr
   exit 1
fi

DATA_NAME=$1$2
mkdir -p tmp
mkdir -p "data/$DATA_NAME"

for f in data/$1/f*
do
  id=$(basename $f)
  echo $id $(readlink -f $f)
done > "tmp/$1_list"

head -$2 "tmp/$1_list" > "tmp/$1_list$2"

$FSM_PATH/fsm-lite -l tmp/$1_list$2 -t tmp -m 21 -M 41 -s 5 -S 95 > data/$DATA_NAME/$DATA_NAME.mat
rm tmp/$1_list
rm tmp/$1_list$2
