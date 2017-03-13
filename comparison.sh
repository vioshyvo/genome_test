if [ ! -f "parameters/$1.sh" ]; then
    echo Invalid data set 1>&2
    exit
fi

. "parameters/$1.sh"

mkdir -p "results/$DATASET_NAME"
for K in 1 10 100; do
    ./exact/tester $DATA_FILE $DATASET_NAME $N $N_TEST $K  > "results/$DATASET_NAME/truth_$K"
done


