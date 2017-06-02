#!/bin/bash

if [ ! -d sdsl-lite ]; then
  git clone https://github.com/simongog/sdsl-lite.git
  pushd sdsl-lite
  ./install.sh .
  popd
fi

if [ ! -d fsm-lite ]; then
  git clone https://github.com/nvalimak/fsm-lite.git
  patch fsm-lite/Makefile patches/fsm1.patch
  pushd fsm-lite
  make depend && make
  popd
fi
