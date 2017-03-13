if [ ! -f "parameters/$1.sh" ]; then
    echo Invalid data set 1>&2
    exit
fi

. "parameters/$1.sh"


echo -n > "results/$DATASET_NAME/mrpt-voting.txt"
for n_trees in $MRPT_VOTING_N_TREES; do
    for depth in $MRPT_DEPTH; do
        ./bin/Release/Sanger $DATA_FILE $DATASET_NAME $N $N_TEST 10 $n_trees $depth "/home/hyvi/HYVI/git/Sanger/results/$DATASET_NAME/" $MRPT_VOTES  >> "results/$DATASET_NAME/mrpt-voting.txt"
    done
done



