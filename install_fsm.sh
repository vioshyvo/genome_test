#!/bin/bash

if [ ! -d sdsl-lite ]; then
  git clone https://github.com/simongog/sdsl-lite.git
  pushd sdsl-lite
  ./install.sh .
  popd
fi
