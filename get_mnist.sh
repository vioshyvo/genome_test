#!/usr/bin/env bash

REMOVE_DOWNLOADED=true # remove downloaded datasets after they've been converted
TEST_N=100 # number of test queries

if [ ! -f "data/mnist/data.bin" ]; then
    mkdir -p data/mnist
    echo "Downloading MNIST..."
    wget "http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz" -O train-images-idx3-ubyte.gz
    echo "Extracting MNIST..."
    gunzip train-images-idx3-ubyte.gz
    echo "Converting MNIST..."
    python2 tools/binary_converter.py train-images-idx3-ubyte data/mnist/data.bin
    python2 tools/binary_converter.py --sample data/mnist/data.bin data/mnist/train.bin data/mnist/test.bin $TEST_N 784
    if [ "$REMOVE_DOWNLOADED" = true ]; then
        rm train-images-idx3-ubyte
    fi
fi
