#!/bin/bash
if [ "$#" -ne "2" ]; then
   echo "error: Expecting parameters: <zip-file> <data-name>" 1>&2
   exit 1
fi

OUTPUT_DIR="data/$2"
mkdir -p "$OUTPUT_DIR"
tar -zxf "$1" -C "$OUTPUT_DIR" --strip-components 1 # unzip straight into OUTPUT_DIR

# print the original file names into the list
ls "$OUTPUT_DIR" > "data/$2_file_list"

# rename the files with a counter
a=1
for i in "$OUTPUT_DIR"/*.fasta; do
  new=$(printf "f%04d" "$a") #04 pad to length of 4
  mv -- "$i" "$OUTPUT_DIR/$new"
  let a=a+1
done
