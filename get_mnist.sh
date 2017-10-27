#!/usr/bin/env bash

REMOVE_DOWNLOADED=true # remove downloaded datasets after they've been converted

if [ ! -f "data/mnist/data.bin" ]; then
    mkdir -p data/mnist
    echo "Downloading MNIST..."
    wget "http://yann.lecun.com/exdb/mnist/train-images-idx3-ubyte.gz" -O train-images-idx3-ubyte.gz
    echo "Extracting MNIST..."
    gunzip train-images-idx3-ubyte.gz
    echo "Converting MNIST..."
    python2 tools/binary_converter.py train-images-idx3-ubyte data/mnist/data.bin
    if [ "$REMOVE_DOWNLOADED" = true ]; then
        rm train-images-idx3-ubyte
    fi
fi
