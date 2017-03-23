if [ ! -f "parameters/$1.sh" ]; then
    echo Invalid data set 1>&2
    exit
fi

MMAP=1

ADD=""
if [[ $MMAP -eq 1 ]]
then
  ADD="_mmap"
fi

. "parameters/$1.sh"


echo "ADD: $ADD"
mkdir -p "results/$DATASET_NAME$ADD"
for K in 1 10 100; do
    ./exact/tester $N $N_TEST 10 $DIM $MMAP "/home/hyvi/HYVI/data/Sanger/Ecoli/$DATASET_NAME/$DATASET_NAME" > "results/$DATASET_NAME$ADD/truth_$K"
done
