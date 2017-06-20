#!/bin/bash

PROGRAM=cmake
PROGRAM_DIR="cmake-3.8.2-Linux-x86_64"
PROGRAM_URL="https://cmake.org/files/v3.8"
if [ $(command -v "$PROGRAM") ]; then
  echo Checking dependencies: "$PROGRAM" is installed.
else
  if [ -d "$PROGRAM_DIR" ]; then
    echo Checking dependencies: "$PROGRAM" is installed in "$PROGRAM_DIR", but not in path.
  else
    echo Checking dependencies: "$PROGRAM" is not installed.
    echo Installing "$PROGRAM" into "$PROGRAM_DIR"...
    wget "$PROGRAM_URL/$PROGRAM_DIR.tar.gz"
    tar -zxf "$PROGRAM_DIR.tar.gz"
    rm "$PROGRAM_DIR.tar.gz"
  fi
  echo Adding directory "$PROGRAM_DIR" into path.
  CRNT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
  export PATH="$PATH:$CRNT_DIR/$PROGRAM_DIR/bin"
fi


if [ ! -d sdsl-lite ]; then
  git clone https://github.com/simongog/sdsl-lite.git
  pushd sdsl-lite
  ./install.sh .
  popd
else
  echo sdsl-lite is already installed.
fi

if [ ! -d fsm-lite ]; then
  git clone https://github.com/nvalimak/fsm-lite.git
  PROGRAM=patch
  if [ $(command -v $PROGRAM) ]; then
    patch fsm-lite/Makefile patches/fsm1.patch
  else
    echo Checking dependencies: "$PROGRAM" not installed in system.
    echo Overwriting  fsm-lite/Makefile with patches/Makefile...
    cp patches/Makefile fsm-lite/Makefile
  fi
  pushd fsm-lite
  make depend && make
  popd
else
  echo fsm-lite is already installed.
fi

if [ ! -d eigen ]; then
  echo Installing Eigen.
  wget http://bitbucket.org/eigen/eigen/get/3.3.4.tar.bz2 -P tmp/
  mkdir eigen
  tar xfj tmp/3.3.4.tar.bz2 -C eigen --strip-components=1
  echo Eigen installed.
else
  echo Eigen 3.3.4 already installed
fi
