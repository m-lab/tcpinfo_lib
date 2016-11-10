#!/bin/bash
# Check out, build, and install protobuf support.
# Building from source as that is recommended in protobuf README.md file.

set -x
# check to see if protobuf tools already exist.
LIBS=`/usr/bin/pkg-config --libs protobuf`
if [ $? -ne 0 ] ; then
  if [ -r protobuf/configure ] ; then
    echo "Using cached protobuf."
    cd protobuf
    sudo make install && sudo ldconfig
  else
    set -e
    echo "Install protobuf from web."
    git clone https://github.com/google/protobuf.git
    # For unknown reasons, compiling protobuf on travis-ci breaks with clang.
    # So we override to always use gcc.
    CC=gcc
    CXX=g++
    cd protobuf
    git checkout v3.1.0
    ./autogen.sh
    ./configure --prefix=/usr
    make
    sudo make install
    sudo ldconfig
  fi
else
  echo "Protobuf already installed."
fi
