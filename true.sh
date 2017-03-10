if [ ! -f "parameters/$1.sh" ]; then
    echo Invalid data set 1>&2
    exit
fi

. "parameters/$1.sh"

mkdir -p "results/$DATASET_NAME"
for K in 1 10 100; do
    ./exact/tester "/home/hyvi/HYVI/git/MRPT_test/data/$DATASET_NAME/train.bin" "/home/hyvi/HYVI/git/MRPT_test/data/$DATASET_NAME/test.bin" $DIM $K > "/home/hyvi/HYVI/git/MRPT_test/data/$DATASET_NAME/truth_$K"
done
