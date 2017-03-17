#!/bin/bash

rm $1/*spades.fa

# print the original file names into the list
ls $1 > file_list

# rename the files with a counter
a=1
for i in $1/*.fa; do
  new=$(printf "f%04d" "$a") #04 pad to length of 4
  mv -- "$i" "$1/$new"
  let a=a+1
done
